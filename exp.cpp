#include "utils.hpp"
#include "stage2/utils2.hpp"

extern std::vector<Value> temp_for_initval;
extern std::vector<int> temp_for_initarray;
extern std::map<std::string, std::vector<std::pair<int,int> > > Ident2Tid; //ident, id, layer
extern std::map<int, Value> VarType; //tid, Value( id value/index/0, type )
extern std::map<int, std::vector<int> > VarArray;
extern std::vector<std::pair<std::string, int> > IdentStack;//ident, layer
extern int Layer, TID, tempID;

//static std::vector<Value> temp_for_exparray;

void transFuncRParams(Type ptr);
void transFuncRParamsTemp(Type ptr);
extern std::map<std::string, bool> FuncRet; //didn't clean because it change with IdentFunc. Can not use alone
extern std::map<int,std::map<int,int> > ConstArray;

Value transNumber(Type ptr);
char transUnaryOp(Type ptr);
char transMulOp(Type ptr);
Value transExp(Type ptr);
int transConstExp(Type ptr);
Value transLVal(Type ptr);
void transLValArrayTemp(Type ptr, std::vector<Value> &temp_for_exparray);
Value transPrimaryExp(Type ptr);
Value transUnaryExp(Type ptr);
Value transMulExp(Type ptr);
Value transAddExp(Type ptr);

/*
func call
FuncRParams:
	{}
	| FuncRParamsTemp
	{};

FuncRParamsTemp:
	Exp
	{
		fprintf(yyout, "var t%d\n", tempVarID);
		fprintf(yyout, "t%d = %d\n", tempVarID, $1.first.first);
		fprintf(yyout, "param t%d\n", tempVarID);
		tempVarID++;				
	}	
	| FuncRParamsTemp COMMA Exp
	{
		fprintf(yyout, "var t%d\n", tempVarID);
		fprintf(yyout, "t%d = %d\n", tempVarID, $3.first.first);
		fprintf(yyout, "param t%d\n", tempVarID);
		tempVarID++;			
	};
*/

Value transNumber(Type ptr){
	int val = ptr.first->SonNode[0].first->token.m_nINT;
	return std::make_pair(std::make_pair(0,val), NumType::m_const);
}

char transUnaryOp(Type ptr){
	return ptr.first->SonNode[0].first->token.m_cOP;
}

char transMulOp(Type ptr){
	char ch = ptr.first->SonNode[0].first->token.m_cOP;
	return ch;
}

Value transExp(Type ptr){
	return transAddExp(ptr.first->SonNode[0]);
}

int transConstExp(Type ptr){
	Value val = transAddExp(ptr.first->SonNode[0]);
	if (val.second != NumType::m_const){
		myerror(val.second);
		yyerror("constexp error: expect a const value");	
	}
	return val.first.second;
}

Value transLVal(Type ptr){
	using std::string;
//	IDENT LValArrayTemp
//didn't check if it declare
	std::vector<Value> temp_for_exparray = std::vector<Value>();

	string str = ptr.first->SonNode[0].first->token.m_sID;

	transLValArrayTemp(ptr.first->SonNode[1], temp_for_exparray);

	int uid = Ident2Tid[ptr.first->SonNode[0].first->token.m_sID].back().first;
	Value ret;

/*
if (str == "a"){
	printf("LVAL: %d\n", temp_for_exparray.size());
	printf("%d %d %d\n", temp_for_exparray[0].first.first, temp_for_exparray[0].first.second, temp_for_exparray[0].second == NumType::m_func_var);
}
*/
	if (temp_for_exparray.size() == 0){
		if (Layer == 0 && (VarType[uid].second == NumType::m_func_var || VarType[uid].second == NumType::m_const || VarType[uid].second == NumType::m_var)){
			ret = VarType[uid];
			ret.second = NumType::m_const;
		}
		else if (VarType[uid].second == NumType::m_const || VarType[uid].second == NumType::m_var){
			ret = VarType[uid];
		}
		else if (VarType[uid].second == NumType::m_func_var){
			ret = VarType[uid];
			ret.first.first = ret.first.second;
		}
		else {
			if (VarType[uid].second == NumType::m_array_var || VarType[uid].second == NumType::m_array_const){
				ret = VarType[uid];
				ret.second = NumType::m_array_id;
			}
			else if (VarType[uid].second == NumType::m_func_array){
				ret = VarType[uid];
				ret.first.first = ret.first.second;
				ret.second = NumType::m_func_id;
			}
			else{
/*
				if (VarType[uid].second == NumType::m_func_id){yyerror("m_func_id");}
				if (VarType[uid].second == NumType::m_array_id){yyerror("m_array_id");}
				if (VarType[uid].second == NumType::m_null){yyerror("m_null");}
*/	
				string err = "LVal error: ident "+str+" is not a lval";
				yyerror(err.c_str());
			}
		}
	}
	else{
		int sum = 0;
		int cal_id = -1;
		// how to use a array with address

		for (int i = 0;i < temp_for_exparray.size();i++){
			switch (temp_for_exparray[i].second){
				case NumType::m_const:
					sum += temp_for_exparray[i].first.second * VarArray[uid][i+1];
					break;
				case NumType::m_var:
					if (cal_id == -1){
						if (opt == PrintOPT::Var)	fprintf(yyout, "var t%d\n", tempID);
						cal_id = tempID;
						tempID++;
						if (opt == PrintOPT::Stmt)	fprintf(yyout, "t%d = t%d * %d\n", cal_id, temp_for_exparray[i].first.first, VarArray[uid][i+1]*4);
					}
					else{
						if (opt == PrintOPT::Stmt)	fprintf(yyout, "t%d = t%d * %d\n", temp_for_exparray[i].first.first, temp_for_exparray[i].first.first, VarArray[uid][i+1]*4);
						if (opt == PrintOPT::Stmt)	fprintf(yyout, "t%d = t%d + t%d\n", cal_id, cal_id, temp_for_exparray[i].first.first);						
					}
					break;	
				case NumType::m_array_var:
				case NumType::m_array_const:
					//m_array can only show in `Lval =` 
					//didn't finish
					if (cal_id == -1){
						if (opt == PrintOPT::Var)	fprintf(yyout, "var t%d\n", tempID);
						cal_id = tempID;
						tempID++;
						if (opt == PrintOPT::Stmt)	fprintf(yyout, "t%d = T%d[%d] * %d\n", cal_id, temp_for_exparray[i].first.first, temp_for_exparray[i].first.second, VarArray[uid][i+1]*4);
					}
					else{
						if (opt == PrintOPT::Stmt)	fprintf(yyout, "t%d = t%d * %d\n", temp_for_exparray[i].first.first, temp_for_exparray[i].first.first, VarArray[uid][i+1]*4);
						if (opt == PrintOPT::Stmt)	fprintf(yyout, "t%d = t%d + t%d\n", cal_id, cal_id, temp_for_exparray[i].first.first);						
					}						
					break;

				case NumType::m_func_array:
					//m_array can only show in `Lval =` 
					//didn't finish
					if (cal_id == -1){
						if (opt == PrintOPT::Var)	fprintf(yyout, "var t%d\n", tempID);
						cal_id = tempID;
						tempID++;
						if (opt == PrintOPT::Stmt)	fprintf(yyout, "t%d = p%d[%d] * %d\n", cal_id, temp_for_exparray[i].first.first, temp_for_exparray[i].first.second, VarArray[uid][i+1]*4);
					}
					else{
						if (opt == PrintOPT::Stmt)	fprintf(yyout, "t%d = t%d * %d\n", temp_for_exparray[i].first.first, temp_for_exparray[i].first.first, VarArray[uid][i+1]*4);
						if (opt == PrintOPT::Stmt)	fprintf(yyout, "t%d = t%d + t%d\n", cal_id, cal_id, temp_for_exparray[i].first.first);						
					}						
					break;

				default:
					break;
			}
		}
		if (cal_id == -1 && (VarType[uid].second == NumType::m_array_const || (Layer == 0 && VarType[uid].second == NumType::m_array_var))){
			int val = ConstArray[uid][sum*4];
			ret = std::make_pair(std::make_pair(0, val), NumType::m_const);
		}
		else if (cal_id == -1){
			if (opt == PrintOPT::Var)	fprintf(yyout, "var t%d\n", tempID);
			if (opt == PrintOPT::Stmt)	fprintf(yyout, "t%d = %d\n", tempID, sum * 4);
			cal_id = tempID;
			tempID++;
			ret = std::make_pair(std::make_pair(VarType[uid].first.first, cal_id), VarType[uid].second);
		}
		else{
			if (sum){
				if (opt == PrintOPT::Stmt)	fprintf(yyout, "t%d = t%d + %d\n", cal_id, cal_id, sum*4);
			}
			ret = std::make_pair(std::make_pair(VarType[uid].first.first, cal_id), VarType[uid].second);
		}

	}
	return ret;
}
void transLValArrayTemp(Type ptr, std::vector<Value> &temp_for_exparray){
	if (ptr.first->SonNode.size() == 0) return;
//LValArrayTemp LEFT_BRACKET Exp RIGHT_BRACKET
	transLValArrayTemp(ptr.first->SonNode[0], temp_for_exparray);
	auto ret = transExp(ptr.first->SonNode[2]);
	temp_for_exparray.push_back(ret);	
}

Value transPrimaryExp(Type ptr){
	using std::make_pair;

	Value ret;
	if (ptr.first->SonNode.size() == 3){
		return transExp(ptr.first->SonNode[1]);
	}
	else if (ptr.first->SonNode[0].second == Label::LVal){
		auto res = transLVal(ptr.first->SonNode[0]);
		if (res.second == NumType::m_array_var || res.second == NumType::m_array_const){
			if (opt == PrintOPT::Var) fprintf(yyout, "var t%d\n", tempID);
			if (opt == PrintOPT::Stmt) fprintf(yyout, "t%d = T%d[t%d]\n", tempID, res.first.first, res.first.second);		
			ret = std::make_pair(std::make_pair(tempID, 0), NumType::m_var);
			tempID++;		
		}
		else if (res.second == NumType::m_func_array){
			if (opt == PrintOPT::Var) fprintf(yyout, "var t%d\n", tempID);
			if (opt == PrintOPT::Stmt) fprintf(yyout, "t%d = p%d[t%d]\n", tempID, res.first.first, res.first.second);		
			ret = std::make_pair(std::make_pair(tempID, 0), NumType::m_var);
			tempID++;		
		}
		else if (res.second == NumType::m_array_id || res.second == NumType::m_func_id){
			ret = res;
		}
		else if (res.second == NumType::m_func_var){
			if (opt == PrintOPT::Var) fprintf(yyout, "var t%d\n", tempID);
			if (opt == PrintOPT::Stmt) fprintf(yyout, "t%d = p%d\n", tempID, res.first.first);
			ret = std::make_pair(std::make_pair(tempID, 0), NumType::m_var);
			tempID++;
		}
		else if (res.second == NumType::m_const){
			ret = res;
		}
		else {
			if (opt == PrintOPT::Var) fprintf(yyout, "var t%d\n", tempID);
			if (opt == PrintOPT::Stmt) fprintf(yyout, "t%d = T%d\n", tempID, res.first.first);
			ret = std::make_pair(std::make_pair(tempID, 0), NumType::m_var);
			tempID++;
		}
		return ret;
	}
	else if (ptr.first->SonNode[0].second == Label::Number){
		return transNumber(ptr.first->SonNode[0]);
	}
}
void transFuncRParams(Type ptr){
//there should be param check, 

	if (ptr.first->SonNode.size() == 0) {
		return;
	}	
	else {
		transFuncRParamsTemp(ptr.first->SonNode[0]);
	}
}
void transFuncRParamsTemp(Type ptr){

	if (ptr.first->SonNode[0].second == Label::Exp){
//Exp
		Value S1 = transExp(ptr.first->SonNode[0]);

		if (S1.second == NumType::m_const){
			if (opt == PrintOPT::Stmt) fprintf(yyout, "param %d\n", S1.first.second);
		}
		else if (S1.second == NumType::m_var){
			if (opt == PrintOPT::Stmt) fprintf(yyout, "param t%d\n", S1.first.first);
		}
		else if (S1.second == NumType::m_array_id){
			if (opt == PrintOPT::Stmt) fprintf(yyout, "param T%d\n", S1.first.first);		
		}
		else if (S1.second == NumType::m_func_id){
			if (opt == PrintOPT::Stmt) fprintf(yyout, "param p%d\n", S1.first.first);		
		}		
		else {
			yyerror("unidentified value in transFunctionRParamsTemp");		
		}
	}
	else{
//FuncRParamsTemp COMMA Exp	
		transFuncRParamsTemp(ptr.first->SonNode[0]);
		Value S1 = transExp(ptr.first->SonNode[2]);

		if (S1.second == NumType::m_const){
			if (opt == PrintOPT::Stmt) fprintf(yyout, "param %d\n", S1.first.second);
		}
		else if (S1.second == NumType::m_var){
			if (opt == PrintOPT::Stmt) fprintf(yyout, "param t%d\n", S1.first.first);
		}
		else if (S1.second == NumType::m_array_id){
			if (opt == PrintOPT::Stmt) fprintf(yyout, "param T%d\n", S1.first.first);		
		}
		else if (S1.second == NumType::m_func_id){
			if (opt == PrintOPT::Stmt) fprintf(yyout, "param p%d\n", S1.first.first);		
		}		
		else {
			yyerror("unidentified value in transFunctionRParamsTemp");		
		}
	}
}
/*
FuncRParams:
	{}
	| FuncRParamsTemp
	{};

FuncRParamsTemp:
	Exp
	{
		fprintf(yyout, "var t%d\n", tempVarID);
		fprintf(yyout, "t%d = %d\n", tempVarID, $1.first.first);
		fprintf(yyout, "param t%d\n", tempVarID);
		tempVarID++;				
	}	
	| FuncRParamsTemp COMMA Exp
	{
		fprintf(yyout, "var t%d\n", tempVarID);
		fprintf(yyout, "t%d = %d\n", tempVarID, $3.first.first);
		fprintf(yyout, "param t%d\n", tempVarID);
		tempVarID++;			
	};
*/

Value transUnaryExp(Type ptr){
	using std::string;

	if (ptr.first->SonNode.size() == 1){
		return transPrimaryExp(ptr.first->SonNode[0]);	
	}
	else if (ptr.first->SonNode.size() == 4){
//func call

// IDENT LEFT_PARENTHESIS FuncRParams RIGHT_PARENTHESIS
		string S1 = ptr.first->SonNode[0].first->token.m_sID;


		transFuncRParams(ptr.first->SonNode[2]);
		if (S1 == "starttime"|| S1 == "stoptime"){
			Value ret;
			if (opt == PrintOPT::Stmt)	fprintf(yyout, "param %d\n", lineNo);
			if (opt == PrintOPT::Stmt)	fprintf(yyout, "call f__sysy_%s\n", S1.c_str());
			ret.second = NumType::m_null;
		}
		else if (FuncRet[S1]){
			if (opt == PrintOPT::Var)	fprintf(yyout, "var t%d\n", tempID);
			if (opt == PrintOPT::Stmt)	fprintf(yyout, "t%d = call f_%s\n", tempID, S1.c_str());
			Value ret = std::make_pair(std::make_pair(tempID,0), NumType::m_var);
			tempID++;
			return ret;		
		}
		else{
			if (opt == PrintOPT::Stmt)	fprintf(yyout, "call f_%s\n", S1.c_str());
			Value ret = std::make_pair(std::make_pair(0,0), NumType::m_null);
			return ret;				
		}
	}
	else{
// UnaryOp UnaryExp
		char S1 = transUnaryOp(ptr.first->SonNode[0]);
		auto S2 = transUnaryExp(ptr.first->SonNode[1]);
		Value ret;
		if (S2.second == NumType::m_const || S2.second == NumType::m_array_id){
			ret = S2;	
			if (S1 == '-') ret.first.second = -S2.first.second;
			if (S1 == '+') ret.first.second = S2.first.second;
			if (S1 == '!') ret.first.second = !S2.first.second;
		}
		else{
			if (opt == PrintOPT::Var) fprintf(yyout, "var t%d\n", tempID);
			if (S1 != '+')
				if (opt == PrintOPT::Stmt) fprintf(yyout, "t%d = %c t%d\n", tempID, S1, S2.first.first);
			ret = std::make_pair(std::make_pair(tempID,0), NumType::m_var);
			tempID++;
		}	
		return ret;
	}
}
Value transMulExp(Type ptr){
	if (ptr.first->SonNode.size() == 1){
		return transUnaryExp(ptr.first->SonNode[0]);	
	}
	else{
		//MulExp MulOp UnaryExp
		int flag = 3;
		int u,v;
		auto S1 = transMulExp(ptr.first->SonNode[0]);
		auto S2 = transMulOp(ptr.first->SonNode[1]);
		auto S3 = transUnaryExp(ptr.first->SonNode[2]);
//some error may happen, if S3 is array id, itw will fault;

		Value ret;
		if (S1.second == NumType::m_const) u = S1.first.second;
		else {u = S1.first.first; flag -= 1;}
		if (S3.second == NumType::m_const) v = S3.first.second;
		else {v = S3.first.first; flag -= 2;}
		switch (flag){
			case 0:
				if (opt == PrintOPT::Var) 
					fprintf(yyout, "var t%d\n", tempID);
				if (opt == PrintOPT::Stmt) fprintf(yyout, "t%d = t%d %c t%d\n", tempID, u, S2, v);
				ret = std::make_pair(std::make_pair(tempID,0), NumType::m_var);
				tempID++;	
				break;
			case 1:
				if (opt == PrintOPT::Var) 
					fprintf(yyout, "var t%d\n", tempID);
				if (opt == PrintOPT::Stmt) fprintf(yyout, "t%d = %d %c t%d\n", tempID, u, S2, v);
				ret = std::make_pair(std::make_pair(tempID,0), NumType::m_var);
				tempID++;					
				break;
			case 2:
				if (opt == PrintOPT::Var) 
					fprintf(yyout, "var t%d\n", tempID);
				if (opt == PrintOPT::Stmt) fprintf(yyout, "t%d = t%d %c %d\n", tempID, u, S2, v);
				ret = std::make_pair(std::make_pair(tempID,0), NumType::m_var);
				tempID++;	
				break;
			case 3:
				int res;
				switch(S2){
					case '*':
						res = u*v;
						break;
					case '/':
						res = u/v;
						break;
					case '%':
						res = u%v;
						break;
					default:
						myerror("mulexp error");
				}
				ret = std::make_pair(std::make_pair(0,res), NumType::m_const);
				break;
			default:
				myerror("mulexp error");		
		}		
		return ret;
	}
}
Value transAddExp(Type ptr){
	Value ret;
	if (ptr.first->SonNode.size() == 1){
		auto ret = transMulExp(ptr.first->SonNode[0]);	
		return ret;
	}
	else{
		//AddExp UnaryOp MulExp
		auto S1 = transAddExp(ptr.first->SonNode[0]);
		auto S2 = transUnaryOp(ptr.first->SonNode[1]);
		auto S3 = transMulExp(ptr.first->SonNode[2]); 

		if (S2 == '!'){
			myerror("addexp error: no `!` in this expression");		
		}

		int flag = 3;
		int u,v;
		if (S1.second == NumType::m_const) u = S1.first.second;
		else {u = S1.first.first; flag -= 1;}
		if (S3.second == NumType::m_const) v = S3.first.second;
		else {v = S3.first.first; flag -= 2;}		

		switch (flag){
			case 0:
				if (opt == PrintOPT::Var) fprintf(yyout, "var t%d\n", tempID);
				if (opt == PrintOPT::Stmt) fprintf(yyout, "t%d = t%d %c t%d\n", tempID, u, S2, v);
				ret = std::make_pair(std::make_pair(tempID,0), NumType::m_var);
				tempID++;	
				break;
			case 1:
				if (opt == PrintOPT::Var) fprintf(yyout, "var t%d\n", tempID);
				if (opt == PrintOPT::Stmt) fprintf(yyout, "t%d = %d %c t%d\n", tempID, u, S2, v);
				ret = std::make_pair(std::make_pair(tempID,0), NumType::m_var);
				tempID++;
				break;
			case 2:
				if (opt == PrintOPT::Var) fprintf(yyout, "var t%d\n", tempID);
				if (opt == PrintOPT::Stmt) fprintf(yyout, "t%d = t%d %c %d\n", tempID, u, S2, v);
				ret = std::make_pair(std::make_pair(tempID,0), NumType::m_var);
				tempID++;	
				break;
			case 3:
				int res;
				switch(S2){
					case '+':
						res = u+v;
						break;
					case '-':
						res = u-v;
						break;
					case '!':
						myerror("addexp error: no `!` in this expression");
						break;
					default:
						myerror("addexp error");
				}
				ret = std::make_pair(std::make_pair(0,res), NumType::m_const);
				break;
			default:
				myerror("addexp error");		
		}	
	}
	return ret;
}
/**
 * expression end
 */
