#include "utils2.hpp"


namespace stage2{

using namespace std;


extern int paramCount;
extern int stackPos;
extern map<string, tValue> tVar;
extern map<string, tValue> pVar;


vector<regValue> regHeap; 
list<int> regPool;
string regConvert(int id);
int regConvert(string str);

extern string T2v(string);
void writeBack(int,PrintType);//store reg

void loadValue(tValue&, PrintType);//load value to arbitrary reg
void loadValue2(tValue&, PrintType);//arrange a reg to a value (no read)
void loadValue(tValue&, int, PrintType);//load value to some reg

void storeReg(int, tValue&, PrintType);//store reg to stack/global var
int callerSave(PrintType);//store callerSave reg
int calleeSave(PrintType);//store calleeSave reg
void calleeRecover(PrintType);
void callerRecover(PrintType);

/*
x0: 该寄存器恒等于 0, 不可更改
s0-s11: 通用寄存器, 由 “被调用者” (callee) 保存.
t0-t6: 通用寄存器, 由 “调用者” (caller) 保存.
a0-a7: 用来传递函数参数, 同时也可作为通用寄存器, 由 “调用者” 保存. 其中 a0-a1 也被用作传递函数返回值, 但因为 SysY 中所有函数的返回值均为 int 类型, 所以实际上只有 a0 被用作传递返回值.
*/
string regConvert(int id){
	string ret; 
//if (id == 20) printf("error\n");
	if (id == 0) ret = "x0";
	else if (1 <= id && id <= 12) ret = "s" + to_string(id - 1);
	else if (13 <= id && id <= 19) ret = "t" + to_string(id - 13);
	else if (20 <= id && id <= 27) ret = "a" + to_string(id - 20);
	else yyerror("reg id error");	
	return ret;
}
int regConvert(string str){
	if (str == "x0") return 0;
	if (str[0] == 's') return 1 + atoi(str.substr(1).c_str());
	if (str[0] == 't') return 13 + atoi(str.substr(1).c_str());
	if (str[0] == 'a') return 20 + atoi(str.substr(1).c_str());
	yyerror("reg name error");
	return -1;
}


void writeBack(int reg, PrintType type){
/*
	note that even if I don't need to writeback , the relationship
	between reg and variant should be broke up
*/
	bool flag = true;
	if (reg == 0) return;
	auto str = regHeap[reg].ident;
	auto it = (pVar.find(str) != pVar.end()) ? pVar.find(str) : tVar.find(str); 
	if (it == tVar.end()) return;
	if (it->second.reg != reg) return;
	
	if (it->second.onStack && (!regHeap[reg].dirty)) flag = false;
//if (reg == 20) printf("writeback\n");
	if (flag)
	switch(it->second.type){
		case NumType::Var:
		case NumType::FVar:
			if (it->second.onStack == false){
				it->second.onStack = true;
				it->second.addr = stackPos;
				stackPos += 4;
			}
			if (type == PrintType::On){
				if (option == BackEnd::Tigger)
					fprintf(yyout, "store %s %d\n", regConvert(reg).c_str(), it->second.addr/4);
				if (option == BackEnd::RISCV){
					fprintf(yyout, "  sw %s, %d(sp)\n", regConvert(reg).c_str(), it->second.addr/4*4);
				}
			}			
			break;
		case NumType::GVar:
			if (type == PrintType::On){
				if (option == BackEnd::Tigger){
					fprintf(yyout, "loadaddr %s s0\n", T2v(it->second.ident).c_str());
					fprintf(yyout, "s0[0] = %s\n", regConvert(reg).c_str());
				}
				if (option == BackEnd::RISCV){
					fprintf(yyout, "  la s0, %s\n", T2v(it->second.ident).c_str());
					fprintf(yyout, "  sw %s, 0(s0)\n", regConvert(reg).c_str());				
				}
			}
			break;
		default:
			break;
	}
	
	it->second.reg = 0;
	regHeap[reg].ident = "null";
	
}

void loadValue(tValue& val, PrintType type){

	if (val.reg != 0 && regHeap[val.reg].ident == val.ident){
		regPool.remove(val.reg);
		regPool.push_back(val.reg);
		return;			
	}

	auto it = (pVar.find(val.ident) != pVar.end()) ? pVar.find(val.ident) : tVar.find(val.ident); 

	switch (val.type){
		case NumType::Num:{
			auto tmp = regPool.front();regPool.pop_front();
	//		printf("error, %d\n", tmp);
			writeBack(tmp, type);
			regPool.push_back(tmp);
			regHeap[tmp].ident = "null";
			if (type == PrintType::On){
				if (option == BackEnd::Tigger)
					fprintf(yyout, "%s = %d\n", regConvert(tmp).c_str(), val.val);
				if (option == BackEnd::RISCV)
					fprintf(yyout, "  li %s, %d\n", regConvert(tmp).c_str(), val.val);
			}
			val.reg = tmp;
			break;
		}
		case NumType::GVar:{
			if (it == tVar.end()) break;
			auto tmp = regPool.front();regPool.pop_front();
			writeBack(tmp,type);
			if (type == PrintType::On){
				if (option == BackEnd::Tigger)
					fprintf(yyout, "load %s %s\n", T2v(it->second.ident).c_str(), regConvert(tmp).c_str());
				if (option == BackEnd::RISCV){
					fprintf(yyout, "  lui %s, %%hi(%s)\n", regConvert(tmp).c_str(), T2v(it->second.ident).c_str());
					fprintf(yyout, "  lw %s, %%lo(%s)(%s)\n", regConvert(tmp).c_str(), T2v(it->second.ident).c_str(), regConvert(tmp).c_str());
				}								
			}
			regPool.push_back(tmp);
			regHeap[tmp].ident = it->second.ident;
			it->second.reg = tmp;
			val.reg = tmp;
			break;
		}
		case NumType::GArr:{
			if (it == tVar.end()) break;
			auto tmp = regPool.front();regPool.pop_front();
			writeBack(tmp,type);
			if (type == PrintType::On){
				if (option == BackEnd::Tigger)
					fprintf(yyout, "loadaddr %s %s\n", T2v(it->second.ident).c_str(), regConvert(tmp).c_str());
				if (option == BackEnd::RISCV){
					fprintf(yyout, "  la %s, %s\n", regConvert(tmp).c_str(), T2v(it->second.ident).c_str());
					//fprintf(yyout, "  lui %s, %%hi(%s)\n", regConvert(tmp).c_str(), T2v(it->second.ident).c_str());
					//fprintf(yyout, "  lw %s, %%lo(%s)(%s)\n", regConvert(tmp).c_str(), T2v(it->second.ident).c_str(), regConvert(tmp).c_str());
				}	
			}
			regPool.push_back(tmp);
			regHeap[tmp].ident = it->second.ident;
			it->second.reg = tmp;
			val.reg = tmp;
			break;
		}
		case NumType::Var:{
			if (it == tVar.end()) break;
			int tmp = regPool.front();regPool.pop_front();
			writeBack(tmp, type);
			if (type == PrintType::On){
				if (option == BackEnd::Tigger)
					fprintf(yyout, "load %d %s\n", it->second.addr/4, regConvert(tmp).c_str());
				if (option == BackEnd::RISCV){
					fprintf(yyout, "  lw %s, %d(sp)\n", regConvert(tmp).c_str(), it->second.addr/4*4);
				}
			}
			regPool.push_back(tmp);
			regHeap[tmp].ident = it->second.ident;
			it->second.reg = tmp;
			val.reg = tmp;
			break;			
		}
		case NumType::Arr:{
			if (it == tVar.end()) break;
			int tmp = regPool.front();regPool.pop_front();
			writeBack(tmp, type);
			if (type == PrintType::On){
				if (option == BackEnd::Tigger)
					fprintf(yyout, "loadaddr %d %s\n", it->second.addr/4, regConvert(tmp).c_str());
				if (option == BackEnd::RISCV)
					fprintf(yyout, "  addi %s, sp, %d\n", regConvert(tmp).c_str(), it->second.addr/4*4);
			}
			regPool.push_back(tmp);
			regHeap[tmp].ident = it->second.ident;
			it->second.reg = tmp;
			val.reg = tmp;
			break;
		}
		case NumType::FVar:{
			if (it == tVar.end()) break;
			int tmp = regPool.front();regPool.pop_front();
			writeBack(tmp, type);
			if (type == PrintType::On){
				if (option == BackEnd::Tigger)
					fprintf(yyout, "load %d %s\n", it->second.addr/4, regConvert(tmp).c_str());
				if (option == BackEnd::RISCV)
					fprintf(yyout, "  lw %s, %d(sp)\n", regConvert(tmp).c_str(), it->second.addr/4*4);
				
			}
			regPool.push_back(tmp);
			regHeap[tmp].ident = it->second.ident;
			it->second.reg = tmp;
			val.reg = tmp;
			break;	
		}
		default:
//			printf("error value %d %s\n", val.val ,val.ident.c_str());
			yyerror("loadValue error");
			break;
	}
}

void loadValue2(tValue& val, PrintType type){
	loadValue(val,type);
	regHeap[val.reg].dirty = true;
}

void loadValue(tValue& val, int dst, PrintType type){
	if (val.reg == dst && regHeap[dst].ident == val.ident){
		regPool.remove(val.reg);
		regPool.push_back(val.reg);
		return;		
	}

	regPool.remove(dst);
	regPool.push_front(dst);
	if (val.reg != 0 && regHeap[val.reg].ident == val.ident)
		writeBack(val.reg, type);
	val.reg = 0;
	loadValue(val,type);
}
void storeReg(int reg, tValue& val, PrintType type){

	auto it = (pVar.find(val.ident) != pVar.end()) ? pVar.find(val.ident) : tVar.find(val.ident); 
	if (it == tVar.end()) return;
	switch (val.type){
		case NumType::GVar:{
			if (type == PrintType::On){
				if (option == BackEnd::Tigger){
					fprintf(yyout, "loadaddr %s s0\n", T2v(it->second.ident).c_str());
					fprintf(yyout, "s0[0] = %s\n", regConvert(reg).c_str());
				}
				if (option == BackEnd::RISCV){
					fprintf(yyout, "  lui s0, %%hi(%s)\n", T2v(it->second.ident).c_str());
					fprintf(yyout, "  lw s0, %%lo(%s)(s0))\n", T2v(it->second.ident).c_str());
					fprintf(yyout, "  sw %s, 0(s0)\n", regConvert(reg).c_str());			
				}				
			}
			it->second.reg = 0;
			val.reg = 0;
			break;
		}
		case NumType::Var:
		case NumType::FVar:
		{
			if (it->second.onStack == false){
				it->second.onStack = true;
				it->second.addr = stackPos;
				stackPos += 4;
			}
			if (type == PrintType::On){
//				fprintf(yyout, "store %s %d\n", regConvert(reg).c_str(), it->second.addr/4);
				if (option == BackEnd::Tigger)
					fprintf(yyout, "store %s %d\n", regConvert(reg).c_str(), it->second.addr/4);
				if (option == BackEnd::RISCV){
					fprintf(yyout, "  sw %s, %d(sp)\n", regConvert(reg).c_str(), it->second.addr/4*4);
				}
			}	
			it->second.reg = 0;
			val.reg = 0;		
			break;
		}
		default:
			yyerror("storeValue error");
			break;
	}
}

int callerSave(PrintType type){
	for (auto it: regPool){
	//	printf("reg %s\n",regConvert(it).c_str());
		auto str = regHeap[it].ident;
		if (pVar.find(str) != pVar.end()){
		//	printf("callerSave %s\n", str.c_str());
			if (pVar[str].reg == it) writeBack(it,type);			
		}
		else if (tVar.find(str) != tVar.end()){
			if (tVar[str].reg == it) writeBack(it,type);
		}
	}	
	return 0;
}
int calleeSave(PrintType){
	return 0;
}
void calleeRecover(PrintType){
	return;
}
void callerRecover(PrintType){
	return;
}


}
