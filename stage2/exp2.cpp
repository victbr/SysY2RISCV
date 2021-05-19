
#include "utils2.hpp"


namespace stage2{

using namespace std;


/*
struct rightValue{
	enum RVtype{Ident, Num};
	RVtype type;
	string ident;
	int val;//value or index
	rightValue(RVtype type, string ident, int val):type(type), ident(ident), val(val){	
	}
};
*/



extern int paramCount;
extern int stackPos;
extern map<string, tValue> tVar;
extern map<string, tValue> pVar;

extern int func_stk;

int transExpression(TreeNode*, PrintType);
tValue transRightValue(TreeNode*);
string transBinOp(TreeNode*);


extern vector<regValue> regHeap; 
extern list<int> regPool;
extern string regConvert(int id);
extern int regConvert(string str);


extern void loadValue(tValue&, PrintType);//load value to arbitrary reg
extern void loadValue2(tValue&, PrintType);//arrange a reg to a value (no read)
extern void loadValue(tValue&, int, PrintType);//load value to some reg

extern void storeReg(int, tValue&, PrintType);//store reg to stack/global var
extern int callerSave(PrintType);//store callerSave reg
extern int calleeSave(PrintType);//store calleeSave reg
extern void calleeRecover(PrintType);
extern void callerRecover(PrintType);


tValue transRightValue(TreeNode* ptr){
	if (ptr->SonNode[0].second == Label::SYMBOL){
//SYMBOL
		tValue ret;
		string str = ptr->SonNode[0].first->token.m_str;
		if (pVar.find(str) != pVar.end()) ret = pVar[str];
		else if (tVar.find(str) != tVar.end()) ret = tVar[str];
		else {
			printf("%s\n", str.c_str());
			yyerror("right value error");  
		}
		return ret;
	}
	else{
//NUM
		int val = ptr->SonNode[0].first->token.m_int;
	//	printf("val = %d\n",val);
		return tValue(NumType::Num, val, "int");
	}
}

string transBinOp(TreeNode *ptr){
	return ptr->SonNode[0].first->token.m_str;
/*	OP
	{
		using std::make_pair;
		TreeNode* node = new TreeNode();
		node->SonNode.push_back($1);
		$$ = make_pair(node, Label::BinOp);	
	}
	| LOGICOP
	{
		using std::make_pair;
		TreeNode* node = new TreeNode();
		node->SonNode.push_back($1);
		$$ = make_pair(node, Label::LOGICOP);	
	};
*/
}

int transExpression(TreeNode* ptr, PrintType type){

	if (ptr->SonNode[0].second == Label::SYMBOL && ptr->SonNode[1].second == Label::EQUAL && 
		ptr->SonNode[2].second == Label::RightValue && ptr->SonNode[3].second == Label::BinOp){
//	SYMBOL EQUAL RightValue BinOp RightValue
		auto r1 = transRightValue(ptr->SonNode[2].first);
		auto r2 = transRightValue(ptr->SonNode[4].first);
		auto str = ptr->SonNode[0].first->token.m_str;
		auto op = ptr->SonNode[3].first->token.m_str;
		//printf("binop: %s\n",op.c_str());
		loadValue(r1, type);
		loadValue(r2, type);
		tValue res; 
		if (pVar.find(str) != pVar.end()){
			res = pVar[str];
		}
		else if (tVar.find(str) != tVar.end()){
			res = tVar[str];
		}
		else yyerror("variant didn't declare.");
		loadValue2(res, type);
		if (type == PrintType::On){
			if (option == BackEnd::Tigger)
				fprintf(yyout, "%s = %s %s %s\n", regConvert(res.reg).c_str(), regConvert(r1.reg).c_str(), op.c_str(), regConvert(r2.reg).c_str());
			if (option == BackEnd::RISCV){
				if (op == "+") fprintf(yyout, "  add %s, %s, %s\n", regConvert(res.reg).c_str(), regConvert(r1.reg).c_str(), regConvert(r2.reg).c_str());
				if (op == "-") fprintf(yyout, "  sub %s, %s, %s\n", regConvert(res.reg).c_str(), regConvert(r1.reg).c_str(), regConvert(r2.reg).c_str());
				if (op == "*") fprintf(yyout, "  mul %s, %s, %s\n", regConvert(res.reg).c_str(), regConvert(r1.reg).c_str(), regConvert(r2.reg).c_str());
				if (op == "/") fprintf(yyout, "  div %s, %s, %s\n", regConvert(res.reg).c_str(), regConvert(r1.reg).c_str(), regConvert(r2.reg).c_str());
				if (op == "%") fprintf(yyout, "  rem %s, %s, %s\n", regConvert(res.reg).c_str(), regConvert(r1.reg).c_str(), regConvert(r2.reg).c_str());
				if (op == "<") fprintf(yyout, "  slt %s, %s, %s\n", regConvert(res.reg).c_str(), regConvert(r1.reg).c_str(), regConvert(r2.reg).c_str());
				if (op == ">") fprintf(yyout, "  sgt %s, %s, %s\n", regConvert(res.reg).c_str(), regConvert(r1.reg).c_str(), regConvert(r2.reg).c_str());
				if (op == ">=") fprintf(yyout, "  slt %s, %s, %s\n", regConvert(res.reg).c_str(), regConvert(r2.reg).c_str(), regConvert(r1.reg).c_str());
				if (op == "<=") fprintf(yyout, "  sgt %s, %s, %s\n", regConvert(res.reg).c_str(), regConvert(r2.reg).c_str(), regConvert(r1.reg).c_str());
/**
 * because in my code, result of cond can only be 0 or 1, so no need to trans
 */
				if (op == "&&") fprintf(yyout, "  and %s, %s, %s\n", regConvert(res.reg).c_str(), regConvert(r1.reg).c_str(), regConvert(r2.reg).c_str());
				if (op == "||") fprintf(yyout, "  or %s, %s, %s\n", regConvert(res.reg).c_str(), regConvert(r1.reg).c_str(), regConvert(r2.reg).c_str());
				if (op == "!=") fprintf(yyout, "  xor %s, %s, %s\nsnez %s, %s\n", regConvert(res.reg).c_str(), regConvert(r1.reg).c_str(), regConvert(r2.reg).c_str()
					, regConvert(res.reg).c_str(), regConvert(res.reg).c_str());
				if (op == "==") fprintf(yyout, "  xor %s, %s, %s\nseqz %s, %s\n", regConvert(res.reg).c_str(), regConvert(r1.reg).c_str(), regConvert(r2.reg).c_str()
					, regConvert(res.reg).c_str(), regConvert(res.reg).c_str());
			}
		}
		regHeap[res.reg].dirty = true;
	}
	else if (ptr->SonNode[0].second == Label::SYMBOL && ptr->SonNode[1].second == Label::EQUAL && 
		ptr->SonNode[2].second == Label::OP){ 
//	SYMBOL EQUAL OP RightValue
		auto r1 = transRightValue(ptr->SonNode[3].first);
		auto str = ptr->SonNode[0].first->token.m_str;
		auto op = ptr->SonNode[2].first->token.m_str;
		loadValue(r1, type);
		tValue res; 
		if (pVar.find(str) != pVar.end()){
			res = pVar[str];
		}
		else if (tVar.find(str) != tVar.end()){
			res = tVar[str];
		}
		else yyerror("variant didn't declare.");
		loadValue2(res, type);
		if (type == PrintType::On){
			if (option == BackEnd::Tigger)
				fprintf(yyout, "%s = %s %s\n", regConvert(res.reg).c_str(), op.c_str(), regConvert(r1.reg).c_str());
			if (option == BackEnd::RISCV){
				if (op == "-") fprintf(yyout, "  neg %s, %s\n", regConvert(res.reg).c_str(), regConvert(r1.reg).c_str());			
				if (op == "!") fprintf(yyout, "  seqz %s, %s\n", regConvert(res.reg).c_str(), regConvert(r1.reg).c_str());			
			}
		}
		regHeap[res.reg].dirty = true;
	}
	else if (ptr->SonNode[0].second == Label::SYMBOL && ptr->SonNode[1].second == Label::EQUAL && 
		ptr->SonNode[2].second == Label::RightValue){ 
//	SYMBOL EQUAL RightValue
		auto r1 = transRightValue(ptr->SonNode[2].first);
		auto str = ptr->SonNode[0].first->token.m_str;
		loadValue(r1, type);

		tValue res; 
		if (pVar.find(str) != pVar.end()){
			res = pVar[str];
		}
		else if (tVar.find(str) != tVar.end()){
			res = tVar[str];
		}
		else yyerror("variant didn't declare.");
		loadValue2(res, type);
		if (type == PrintType::On){
			if (option == BackEnd::Tigger)
				fprintf(yyout, "%s = %s\n", regConvert(res.reg).c_str(), regConvert(r1.reg).c_str());
			if (option == BackEnd::RISCV)
				fprintf(yyout, "  mv %s, %s\n", regConvert(res.reg).c_str(), regConvert(r1.reg).c_str());
		}
		regHeap[res.reg].dirty = true;
	}
	else if (ptr->SonNode[0].second == Label::SYMBOL && ptr->SonNode[1].second == Label::LEFT_BRACKET && 
		ptr->SonNode[2].second == Label::RightValue){ 
//	SYMBOL LEFT_BRACKET RightValue RIGHT_BRACKET EQUAL RightValue
		auto r1 = transRightValue(ptr->SonNode[2].first);
		auto r2 = transRightValue(ptr->SonNode[5].first);
		auto str = ptr->SonNode[0].first->token.m_str;
		loadValue(r2, type);
		tValue res; 
		if (pVar.find(str) != pVar.end()){
			res = pVar[str];
		}
		else if (tVar.find(str) != tVar.end()){
			res = tVar[str];
		}
		else yyerror("variant didn't declare.");
		loadValue(res, type);
		if (r1.type == NumType::Num){
			if (type == PrintType::On){
			if (option == BackEnd::Tigger)
				fprintf(yyout, "%s[%d] = %s\n", regConvert(res.reg).c_str(), r1.val, regConvert(r2.reg).c_str());
			if (option == BackEnd::RISCV)
				fprintf(yyout, "  sw %s, %d(%s)\n", regConvert(r2.reg).c_str(), r1.val, regConvert(res.reg).c_str());
			}
			regHeap[res.reg].dirty = true;
		}
		else{
			loadValue(r1, type);
			pVar["temp"] = tValue(NumType::Var,0,"temp");
			auto tmp = pVar["temp"];
			loadValue(tmp, type);
			if (type == PrintType::On){
				if (option == BackEnd::Tigger){
					fprintf(yyout, "%s = %s + %s\n", regConvert(tmp.reg).c_str(), regConvert(res.reg).c_str(), regConvert(r1.reg).c_str());
					fprintf(yyout, "%s[%d] = %s\n", regConvert(tmp.reg).c_str(), 0, regConvert(r2.reg).c_str());
				}
				if (option == BackEnd::RISCV){
					fprintf(yyout, "  add %s, %s, %s\n", regConvert(tmp.reg).c_str(), regConvert(res.reg).c_str(), regConvert(r1.reg).c_str());
					fprintf(yyout, "  sw %s, %d(%s)\n", regConvert(r2.reg).c_str(), 0, regConvert(tmp.reg).c_str());			
				}
			}
			pVar.erase("temp");
		}
	}
	else if (ptr->SonNode[0].second == Label::SYMBOL && ptr->SonNode[1].second == Label::EQUAL && 
		ptr->SonNode[2].second == Label::SYMBOL){ 
//	SYMBOL EQUAL SYMBOL LEFT_BRACKET RightValue RIGHT_BRACKET
		auto s1 = ptr->SonNode[2].first->token.m_str;
		auto r2 = transRightValue(ptr->SonNode[4].first);
		auto str = ptr->SonNode[0].first->token.m_str;
		tValue r1; 
		if (pVar.find(s1) != pVar.end()){
			r1 = pVar[s1];
		}
		else if (tVar.find(s1) != tVar.end()){
			r1 = tVar[s1];
		}
		else yyerror("variant didn't declare.");
		tValue res;	
		if (pVar.find(str) != pVar.end()){
			res = pVar[str];
		}
		else if (tVar.find(str) != tVar.end()){
			res = tVar[str];
		}
		else yyerror("variant didn't declare.");

		loadValue(r1, type);
		loadValue2(res, type);

		if (r2.type == NumType::Num){
			if (type == PrintType::On){
				if (option == BackEnd::Tigger)
					fprintf(yyout, "%s = %s[%d]\n", regConvert(res.reg).c_str(), regConvert(r1.reg).c_str(), r2.val);
				if (option == BackEnd::RISCV)
					fprintf(yyout, "  lw %s, %d(%s)\n", regConvert(res.reg).c_str(), r2.val, regConvert(r1.reg).c_str());	
			}
			regHeap[res.reg].dirty = true;
		}
		else{
			loadValue(r2, type);
			pVar["temp"] = tValue(NumType::Var,0,"temp");
			auto tmp = pVar["temp"];
			loadValue(tmp, type);
			if (type == PrintType::On){
				if (option == BackEnd::Tigger){	
					fprintf(yyout, "%s = %s + %s\n", regConvert(tmp.reg).c_str(), regConvert(r1.reg).c_str(), regConvert(r2.reg).c_str());
					fprintf(yyout, "%s = %s[0]\n", regConvert(res.reg).c_str(), regConvert(tmp.reg).c_str());
				}
				if (option == BackEnd::RISCV){	
					fprintf(yyout, "  add %s, %s, %s\n", regConvert(tmp.reg).c_str(), regConvert(r1.reg).c_str(), regConvert(r2.reg).c_str());
					fprintf(yyout, "  lw %s, 0(%s)\n", regConvert(res.reg).c_str(), regConvert(tmp.reg).c_str());	
				}
			}
			regHeap[res.reg].dirty = true;
			pVar.erase("temp");   
		}
	}
	else if (ptr->SonNode[0].second == Label::IF && ptr->SonNode[1].second == Label::RightValue && 
		ptr->SonNode[2].second == Label::LOGICOP){ 
//	IF RightValue LOGICOP RightValue GOTO LABEL
		callerSave(type);
		auto r1 = transRightValue(ptr->SonNode[1].first);
		auto r2 = transRightValue(ptr->SonNode[3].first);
		string op = ptr->SonNode[2].first->token.m_str;
		string label = ptr->SonNode[5].first->token.m_str;

		loadValue(r1, type);
		loadValue(r2, type);
		if (type == PrintType::On){
			if (option == BackEnd::Tigger)
				fprintf(yyout, "if %s %s %s goto %s\n", regConvert(r1.reg).c_str(), op.c_str(), regConvert(r2.reg).c_str(), label.c_str());
			if (option == BackEnd::RISCV){
				string opt = "err";
				if (op == "<") opt = "blt";
				if (op == ">") opt = "bgt";
				if (op == "<=") opt = "ble";
				if (op == ">=") opt = "bge";
				if (op == "!=") opt = "bne";
				if (op == "==") opt = "beq";				
				fprintf(yyout, "  %s %s, %s, .%s\n", opt.c_str(), regConvert(r1.reg).c_str(), regConvert(r2.reg).c_str(), label.c_str());
			}
		}
	}
	else if (ptr->SonNode[0].second == Label::GOTO && ptr->SonNode[1].second == Label::LABEL){ 
//	GOTO LABEL
		callerSave(type);
		if (type == PrintType::On){
			if (option == BackEnd::Tigger)
				fprintf(yyout, "goto %s\n", ptr->SonNode[1].first->token.m_str.c_str());
			if (option == BackEnd::RISCV)
				fprintf(yyout, "  j .%s\n", ptr->SonNode[1].first->token.m_str.c_str());
		}
	}
	else if (ptr->SonNode[0].second == Label::LABEL && ptr->SonNode[1].second == Label::COLON){ 
//	LABEL COLON
		callerSave(type);
		if (type == PrintType::On) {
			if (option == BackEnd::Tigger)
				fprintf(yyout, "%s:\n", ptr->SonNode[0].first->token.m_str.c_str());
			if (option == BackEnd::RISCV)
				fprintf(yyout, ".%s:\n", ptr->SonNode[0].first->token.m_str.c_str());
		}
	}
	else if (ptr->SonNode[0].second == Label::PARAM && ptr->SonNode[1].second == Label::RightValue){ 
//	PARAM RightValue
		string str = "a" + to_string(paramCount);
		paramCount++;
//printf("param %d\n", paramCount);
//		writeBack(regConvert(str));
		auto r1 = transRightValue(ptr->SonNode[1].first);
		loadValue(r1, regConvert(str), type);
		regHeap[regConvert(str)].ident = "param";
	}
	else if (ptr->SonNode[0].second == Label::CALL && ptr->SonNode[1].second == Label::FUNCTION){ 
//	CALL FUNCTION
		paramCount = 0;
		callerSave(type);
		if (type == PrintType::On) {
			if (option == BackEnd::Tigger)
				fprintf(yyout, "call %s\n", ptr->SonNode[1].first->token.m_str.c_str());
			if (option == BackEnd::RISCV)
				fprintf(yyout, "  call %s\n", ptr->SonNode[1].first->token.m_str.substr(2).c_str());
		}
		callerRecover(type);
	}
	else if (ptr->SonNode[0].second == Label::SYMBOL && ptr->SonNode[1].second == Label::EQUAL && 
		ptr->SonNode[2].second == Label::CALL){ 
//	SYMBOL EQUAL CALL FUNCTION
		auto str = ptr->SonNode[0].first->token.m_str;
		paramCount = 0;
//caller save
//need to complete
		callerSave(type);
		if (type == PrintType::On) {
			if (option == BackEnd::Tigger)
				fprintf(yyout, "call %s\n", ptr->SonNode[3].first->token.m_str.c_str());
			if (option == BackEnd::RISCV)
				fprintf(yyout, "call %s\n", ptr->SonNode[3].first->token.m_str.substr(2).c_str());
		}

		if (pVar.find(str) != pVar.end()){
			storeReg(regConvert("a0"), pVar[str], type);
		}
		else if (tVar.find(str) != tVar.end()){
			storeReg(regConvert("a0"), tVar[str], type);
		}
		else yyerror("variant didn't declare");
		callerRecover(type);
	}
	else if (ptr->SonNode[0].second == Label::RETURN && ptr->SonNode[1].second == Label::RightValue){ 
//	RETURN RightValue NL
		auto res = transRightValue(ptr->SonNode[1].first);
//		writeBack(regConvert("a0"));

		loadValue(res, regConvert("a0"), type);
		calleeRecover(type);
		if (type == PrintType::On){
			if (option == BackEnd::Tigger)
				fprintf(yyout, "return\n");
			if (option == BackEnd::RISCV){
				fprintf(yyout, "  lw ra, %d(sp)\n\
  addi sp, sp, %d\n\
  ret\n", func_stk-4, func_stk);
			}
			
		}

	}
	else if (ptr->SonNode[0].second == Label::RETURN){ 
//	RETURN NL
		if (type == PrintType::On){
			if (option == BackEnd::Tigger)
				fprintf(yyout, "return\n");
			if (option == BackEnd::RISCV){
				fprintf(yyout, "  lw ra, %d(sp)\n\
  addi sp, sp, %d\n\
  ret\n", func_stk-4, func_stk);
			}
			
		}
	}
	else{
		yyerror("expression error");	
	}

}


}
