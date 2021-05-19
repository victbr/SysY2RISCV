#include "utils.hpp"



extern std::map<std::string, std::vector<std::pair<int,int> > > Ident2Tid; //ident, id, layer
extern std::map<int, Value> VarType; //tid, Value( id value/index/0, type )
extern std::map<int, std::vector<int> > VarArray;
extern std::vector<std::pair<std::string, int> > IdentStack;//ident, layer


extern Value transLVal(Type);
extern Value transExp(Type);
extern Value transAddExp(Type);
extern void transDecl(Type);


extern int tempID, Layer, LID;

std::string myConvert(Value val){
	using std::string;
	using std::to_string;
	string ret="";
	switch (val.second){
		case NumType::m_const:
			ret = to_string(val.first.second);	
			break;
		case NumType::m_var:
			ret = "t"+to_string(val.first.first);
			break;
		case NumType::m_array_var:
		case NumType::m_array_const:
			ret = "T"+to_string(val.first.first) + "[t"+to_string(val.first.second)+"]";
			break;

		case NumType::m_array_id:
			ret = "T"+to_string(val.first.first);
			break;		
		default:
			yyerror("convert error");				
	}
	return ret;			
}

std::string myConvertL(Value val){
	using std::string;
	using std::to_string;
	string ret="";
	switch (val.second){
		case NumType::m_const:
			ret = to_string(val.first.second);	
			break;
		case NumType::m_var:
			ret = "T"+to_string(val.first.first);
			break;
		case NumType::m_array_var:
		case NumType::m_array_const:
			ret = "T"+to_string(val.first.first) + "[t"+to_string(val.first.second)+"]";
			break;
		case NumType::m_func_var:
			ret = "p"+to_string(val.first.first);
			break;
		case NumType::m_func_array:
			ret = "p"+to_string(val.first.first) + "[t"+to_string(val.first.second)+"]";
			break;
		case NumType::m_array_id:
			ret = "T"+to_string(val.first.first);
			break;		
		default:
			yyerror("convert error");				
	}
	return ret;			
}


void transStmt(Type,int,int);
void transStmt_Other(Type,int,int);
void transStmt_NoElse(Type,int,int);

Value transCond(Type,int,int);
Value transLOrExp(Type,int,int);
Value transRelExp(Type);
std::string transRelOp(Type);
Value transEqExp(Type,int,int);
Value transLAndExp(Type,int,int);
Value transLOrExp(Type);

void transBlock(Type,int,int);
void transBlockItems(Type,int,int);
void transBlockItem(Type,int,int);

void leaveBlock(){
	while (!IdentStack.empty()){
		auto it = IdentStack.back();
		if (it.second < Layer) break;
		if (it.second > Layer) yyerror("block leave error: some ident didn't clean up");
		int u = Ident2Tid[it.first].back().first;
		int v = Ident2Tid[it.first].back().second;
		if (v != Layer) yyerror("block leave error: ident2tid didn't match identstack");		
		VarType.erase(u);
		VarArray.erase(u);
		IdentStack.pop_back();
		Ident2Tid[it.first].pop_back();
	}
}


void transBlock(Type ptr, int brkptr, int ctnptr){
	Layer++;
//	LEFT_BRACE BlockItems RIGHT_BRACE
	transBlockItems(ptr.first->SonNode[1], brkptr, ctnptr);

	//undo action
	leaveBlock();
	Layer--;
}

void transBlockItems(Type ptr, int brkptr, int ctnptr){
	if (ptr.first->SonNode.size() == 0) return;
	else{
		transBlockItems(ptr.first->SonNode[0], brkptr, ctnptr);
		transBlockItem(ptr.first->SonNode[1], brkptr, ctnptr);
	}
}

void transBlockItem(Type ptr, int brkptr, int ctnptr){

	if (ptr.first->SonNode[0].second == Label::Decl){
		transDecl(ptr.first->SonNode[0]);	
	}
	else if (ptr.first->SonNode[0].second == Label::Stmt){
		transStmt(ptr.first->SonNode[0], brkptr, ctnptr);
	}
	else{
		yyerror("error in block item");
	}
}
void transStmt(Type ptr, int brkptr, int ctnptr){

	switch (ptr.first->SonNode[0].second){
		case Label::Stmt_Other:
			transStmt_Other(ptr.first->SonNode[0], brkptr, ctnptr);
			break;
		case Label::Stmt_NoElse:
			transStmt_NoElse(ptr.first->SonNode[0], brkptr, ctnptr);
			break;		
		default:
			yyerror("error in Stmt");
	}
}

void transStmt_Other(Type ptr, int brkptr, int ctnptr){
	using std::string;

	switch (ptr.first->SonNode[0].second){
		case Label::LVal:
//	LVal EQUAL Exp SEMICOLON
			{

				auto S1 = myConvertL(transLVal(ptr.first->SonNode[0]));
				auto S3 = myConvert(transExp(ptr.first->SonNode[2]));
				if (opt == PrintOPT::Stmt) {
					fprintf(yyout, "%s = %s\n", S1.c_str(), S3.c_str());			
				}
			}
			break;
		case Label::Exp:
			{
// Exp SEMICOLON
				//confuse
				transExp(ptr.first->SonNode[0]);
			}
			break;
		case Label::SEMICOLON:
//SEMICOLON
			break;
		case Label::Block:
			{
				transBlock(ptr.first->SonNode[0], brkptr, ctnptr);
			}
			break;
		case Label::WHILE:
//WHILE LEFT_PARENTHESIS Cond RIGHT_PARENTHESIS Stmt_Other
			{
				if (opt == PrintOPT::Stmt) fprintf(yyout, "l%d:\n",LID);
				int ctn = LID;				
				LID++;
				int brk = LID;				
				LID++;
				int cdT = LID;
				LID++;
				int cdF = brk;
				auto res = transCond(ptr.first->SonNode[2],cdT,cdF);
				std::string str = myConvert(res);
				if (opt == PrintOPT::Stmt)	fprintf(yyout, "if %s == 0 goto l%d\n", str.c_str(), brk);	
				if (opt == PrintOPT::Stmt) fprintf(yyout, "l%d:\n",cdT);
				transStmt_Other(ptr.first->SonNode[4],brk,ctn);
				if (opt == PrintOPT::Stmt) fprintf(yyout, "goto l%d\n",ctn);
				if (opt == PrintOPT::Stmt) fprintf(yyout, "l%d:\n",brk);
			}
			break;
		case Label::BREAK:
			{
				if (opt == PrintOPT::Stmt) fprintf(yyout, "goto l%d\n",brkptr);
			}
			break;
		case Label::CONTINUE:
			{
				if (opt == PrintOPT::Stmt) fprintf(yyout, "goto l%d\n",ctnptr);
			}
			break;	
		case Label::RETURN:
			if (ptr.first->SonNode[1].second == Label::SEMICOLON){
				if (opt == PrintOPT::Stmt)	fprintf(yyout, "return\n");
			}
			else if (ptr.first->SonNode[1].second == Label::Exp){	
//RETURN Exp SEMICOLON
				auto res = myConvert(transExp(ptr.first->SonNode[1]));
				if (opt == PrintOPT::Stmt)	fprintf(yyout, "return %s\n",res.c_str());
			}
			break;
		case Label::IF:
//IF LEFT_PARENTHESIS Cond RIGHT_PARENTHESIS Stmt_Other ELSE Stmt_Other
			{

				int branchTrue = LID;
				LID++;
				int branchOut = LID;
				LID++;
				int cdT = branchTrue;
				int cdF = LID;
				LID++;

				auto res = transCond(ptr.first->SonNode[2],cdT,cdF);
				std::string str = myConvert(res);
				if (opt == PrintOPT::Stmt)	fprintf(yyout, "if %s != 0 goto l%d\n", str.c_str(), branchTrue);
				if (opt == PrintOPT::Stmt) fprintf(yyout, "l%d:\n",cdF);
				transStmt_Other(ptr.first->SonNode[6], brkptr, ctnptr);
				if (opt == PrintOPT::Stmt)	fprintf(yyout, "goto l%d\n", branchOut);
				if (opt == PrintOPT::Stmt)	fprintf(yyout, "l%d:\n", branchTrue);
				transStmt_Other(ptr.first->SonNode[4], brkptr, ctnptr);
				if (opt == PrintOPT::Stmt)	fprintf(yyout, "l%d:\n", branchOut);
			}
			break;
		default:
			yyerror("error in stmt other");
	}


}

void transStmt_NoElse(Type ptr,int brkptr, int ctnptr){
	if (ptr.first->SonNode[0].second == Label::WHILE){
//	| WHILE LEFT_PARENTHESIS Cond RIGHT_PARENTHESIS Stmt_NoElse
		if (opt == PrintOPT::Stmt) fprintf(yyout, "l%d:\n",LID);
		int ctn = LID;				
		LID++;
		int brk = LID;				
		LID++;
		int cdT = LID;
		LID++;
		int cdF = brk;
		auto res = transCond(ptr.first->SonNode[2],cdT,cdF);
		std::string str = myConvert(res);
		if (opt == PrintOPT::Stmt)	fprintf(yyout, "if %s == 0 goto l%d\n", str.c_str(), brk);	
		if (opt == PrintOPT::Stmt) fprintf(yyout, "l%d:\n",cdT);
		transStmt_NoElse(ptr.first->SonNode[4],brk,ctn);
		if (opt == PrintOPT::Stmt) fprintf(yyout, "goto l%d\n",ctn);
		if (opt == PrintOPT::Stmt) fprintf(yyout, "l%d:\n",brk);	
	}
	else if (ptr.first->SonNode.size() == 5){
//	IF LEFT_PARENTHESIS Cond RIGHT_PARENTHESIS Stmt
		int branchTrue = LID;
		LID++;
		int branchOut = LID;
		LID++;
		int cdT = LID;LID++;
		int cdF = branchOut;
		auto res = transCond(ptr.first->SonNode[2],cdT,cdF);
		std::string str = myConvert(res);
		if (opt == PrintOPT::Stmt)	fprintf(yyout, "if %s == 0 goto l%d\n", str.c_str(), branchOut);
		if (opt == PrintOPT::Stmt)	fprintf(yyout, "l%d:\n", cdT);
		transStmt(ptr.first->SonNode[4], brkptr, ctnptr);
		if (opt == PrintOPT::Stmt)	fprintf(yyout, "l%d:\n", branchOut);
	}
	else{
//	| IF LEFT_PARENTHESIS Cond RIGHT_PARENTHESIS Stmt_Other ELSE Stmt_NoElse

		int branchTrue = LID;
		LID++;
		int branchOut = LID;
		LID++;
		int cdT = branchTrue;
		int cdF = LID;LID++;
		auto res = transCond(ptr.first->SonNode[2],cdT,cdF);
		std::string str = myConvert(res);
		if (opt == PrintOPT::Stmt)	fprintf(yyout, "if %s != 0 goto l%d\n", str.c_str(), branchTrue);
		transStmt_Other(ptr.first->SonNode[6], brkptr, ctnptr);
		if (opt == PrintOPT::Stmt)	fprintf(yyout, "goto l%d\n", branchOut);
		if (opt == PrintOPT::Stmt)	fprintf(yyout, "l%d:\n", cdF);
		if (opt == PrintOPT::Stmt)	fprintf(yyout, "l%d:\n", branchTrue);
		transStmt_NoElse(ptr.first->SonNode[4], brkptr, ctnptr);
		if (opt == PrintOPT::Stmt)	fprintf(yyout, "l%d:\n", branchOut);
	}
}


Value transCond(Type ptr, int cdT,int cdF){
	return transLOrExp(ptr.first->SonNode[0],cdT,cdF);
}

Value transRelExp(Type ptr,int cdT,int cdF){
	using std::make_pair;
	using std::string;
	using std::to_string;
	Value ret;
	if (ptr.first->SonNode[0].second == Label::AddExp){
		ret = transAddExp(ptr.first->SonNode[0]);
		string tmp;
		if (ret.second == NumType::m_var) tmp = "t"+to_string(ret.first.first);
		else tmp = to_string(ret.first.second);
		if (opt == PrintOPT::Stmt && cdT != -1) fprintf(yyout, "if %s != 0 goto l%d\n", tmp.c_str(), cdT);
		if (opt == PrintOPT::Stmt && cdF != -1) fprintf(yyout, "if %s == 0 goto l%d\n", tmp.c_str(), cdF);
		return ret;
	}
	else{
		auto S1 = transRelExp(ptr.first->SonNode[0],-1,-1);
		auto S2 = transRelOp(ptr.first->SonNode[1]);
		auto S3 = transAddExp(ptr.first->SonNode[2]);
		
		if (opt == PrintOPT::Var){
			fprintf(yyout, "var t%d\n", tempID);
		}
		string t1;
		string t2 = S2;
		string t3;

		switch (S1.second){
			case NumType::m_const:
				t1 = to_string(S1.first.second);	
				break;
			case NumType::m_var:
				t1 = "t"+to_string(S1.first.first);
			break;		
				default:
				yyerror("relexp");				
		}

		switch (S3.second){
			case NumType::m_const:
				t3 = to_string(S3.first.second);	
				break;
			case NumType::m_var:
				t3 = "t"+to_string(S3.first.first);
				break;		
			default:
				yyerror("relexp");				
		}

		if (opt == PrintOPT::Stmt) fprintf(yyout, "t%d = %s %s %s\n", tempID, t1.c_str(), t2.c_str(), t3.c_str());
		if (opt == PrintOPT::Stmt && cdT != -1) fprintf(yyout, "if t%d != 0 goto l%d\n", tempID, cdT);
		if (opt == PrintOPT::Stmt && cdF != -1) fprintf(yyout, "if t%d == 0 goto l%d\n", tempID, cdF);
		Value ret = make_pair(make_pair(tempID,0), NumType::m_var);
		tempID++;
		return ret;
	}
}

std::string transRelOp(Type ptr){
	using std::make_pair;
	using std::string;
	using std::to_string;

	string ret;

	switch (ptr.first->SonNode[0].second){
		case Label::LES:	ret = "<";	break;
		case Label::GRT:	ret = ">";	break;
		case Label::LEQ:	ret = "<=";	break;
		case Label::GEQ:	ret = ">=";	break;
		default: break;
	}	
	return ret;
}

Value transEqExp(Type ptr, int cdT, int cdF){
	using std::make_pair;
	using std::string;
	using std::to_string;
	Value ret;

	if (ptr.first->SonNode[0].second == Label::RelExp){
		return transRelExp(ptr.first->SonNode[0],cdT,cdF);	
	}
	else {
		auto S1 = transEqExp(ptr.first->SonNode[0],-1,-1);
		string S2 = ptr.first->SonNode[1].second == Label::NEQ ? "!=" : "==";
		auto S3 = transRelExp(ptr.first->SonNode[2],-1,-1);
		
		if (opt == PrintOPT::Var){
			fprintf(yyout, "var t%d\n", tempID);
		}
		string t1;
		string t2 = S2;
		string t3;

		switch (S1.second){
			case NumType::m_const:
				t1 = to_string(S1.first.second);	
				break;
			case NumType::m_var:
				t1 = "t"+to_string(S1.first.first);
			break;		
				default:
				yyerror("eqexp");				
		}

		switch (S3.second){
			case NumType::m_const:
				t3 = to_string(S3.first.second);	
				break;
			case NumType::m_var:
				t3 = "t"+to_string(S3.first.first);
				break;		
			default:
				yyerror("eqexp");				
		}

		if (opt == PrintOPT::Stmt) fprintf(yyout, "t%d = %s %s %s\n", tempID, t1.c_str(), t2.c_str(), t3.c_str());
		if (opt == PrintOPT::Stmt && cdT != -1) fprintf(yyout, "if t%d != 0 goto l%d\n", tempID, cdT);
		if (opt == PrintOPT::Stmt && cdF != -1) fprintf(yyout, "if t%d == 0 goto l%d\n", tempID, cdF);
		Value ret = make_pair(make_pair(tempID,0), NumType::m_var);
		tempID++;
		return ret;
	}
}

Value transLAndExp(Type ptr,int cdT, int cdF){
	using std::make_pair;
	using std::string;
	using std::to_string;
	Value ret;

	if (ptr.first->SonNode[0].second == Label::EqExp){
		return transEqExp(ptr.first->SonNode[0], cdT, cdF);
	}
	else{
		auto S1 = transLAndExp(ptr.first->SonNode[0], -1, cdF);
		auto S2 = "&&";
		auto S3 = transEqExp(ptr.first->SonNode[2], -1, cdF);
		
		if (opt == PrintOPT::Var){
			fprintf(yyout, "var t%d\n", tempID);
		}
		string t1;
		string t2 = S2;
		string t3;

		switch (S1.second){
			case NumType::m_const:
				t1 = to_string(S1.first.second);	
				break;
			case NumType::m_var:
				t1 = "t"+to_string(S1.first.first);
			break;		
				default:
				yyerror("landexp");				
		}

		switch (S3.second){
			case NumType::m_const:
				t3 = to_string(S3.first.second);	
				break;
			case NumType::m_var:
				t3 = "t"+to_string(S3.first.first);
				break;		
			default:
				yyerror("landexp");				
		}

		if (opt == PrintOPT::Stmt) fprintf(yyout, "t%d = %s %s %s\n", tempID, t1.c_str(), t2.c_str(), t3.c_str());
		if (opt == PrintOPT::Stmt && cdT != -1) fprintf(yyout, "if t%d != 0 goto l%d\n", tempID, cdT);
		if (opt == PrintOPT::Stmt && cdF != -1) fprintf(yyout, "if t%d == 0 goto l%d\n", tempID, cdF);
		Value ret = make_pair(make_pair(tempID,0), NumType::m_var);
		tempID++;
		return ret;	
	}
}

Value transLOrExp(Type ptr,int cdT,int cdF){
	using std::make_pair;
	using std::string;
	using std::to_string;
	Value ret;

	if (ptr.first->SonNode[0].second == Label::LAndExp){
		return transLAndExp(ptr.first->SonNode[0], cdT, cdF);
	}
	else{
		auto S1 = transLOrExp(ptr.first->SonNode[0], cdT, -1);
		auto S2 = "||";
		auto S3 = transLAndExp(ptr.first->SonNode[2], cdT, -1);
		
		if (opt == PrintOPT::Var){
			fprintf(yyout, "var t%d\n", tempID);
		}
		string t1;
		string t2 = S2;
		string t3;

		switch (S1.second){
			case NumType::m_const:
				t1 = to_string(S1.first.second);	
				break;
			case NumType::m_var:
				t1 = "t"+to_string(S1.first.first);
			break;		
				default:
				yyerror("lorlexp");				
		}

		switch (S3.second){
			case NumType::m_const:
				t3 = to_string(S3.first.second);	
				break;
			case NumType::m_var:
				t3 = "t"+to_string(S3.first.first);
				break;		
			default:
				yyerror("lorexp");				
		}

		if (opt == PrintOPT::Stmt) fprintf(yyout, "t%d = %s %s %s\n", tempID, t1.c_str(), t2.c_str(), t3.c_str());
		if (opt == PrintOPT::Stmt && cdT != -1) fprintf(yyout, "if t%d != 0 goto l%d\n", tempID, cdT);
		if (opt == PrintOPT::Stmt && cdF != -1) fprintf(yyout, "if t%d == 0 goto l%d\n", tempID, cdF);
		Value ret = make_pair(make_pair(tempID,0), NumType::m_var);
		tempID++;
		return ret;	
	}
}
