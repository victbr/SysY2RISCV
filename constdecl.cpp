#include "utils.hpp"

extern void fuckArray(std::vector<Value>& value, int tid, std::vector<int>& arraySize, std::string str);
extern std::vector<Value> temp_for_initval;
extern std::vector<int> temp_for_initarray;
extern std::map<std::string, std::vector<std::pair<int,int> > > Ident2Tid; //ident, id, layer
extern std::map<int, Value> VarType; //tid, Value( id value/index/0, type )
extern std::map<int, std::vector<int> > VarArray;
extern std::vector<std::pair<std::string, int> > IdentStack;//ident, layer
extern int Layer, TID, tempID;



extern int transArrayTemp(Type ptr);
extern int transConstExp(Type ptr);

void transConstDecl(Type ptr);
void transConstTemp(Type ptr);
void transConstDef(Type ptr);
std::string transConstInitTemp(Type ptr);
std::string transConstInitVal(Type ptr);
std::string transConstArrayInit(Type ptr);




void transConstDecl(Type ptr){
//	CONST BType ConstTemp SEMICOLON
	transConstTemp(ptr.first->SonNode[2]);
}

void transConstTemp(Type ptr){
/*
	ConstDef
	{
	}
	| ConstTemp COMMA ConstDef
	{
	};
*/
	if (ptr.first->SonNode.size() == 1){
		transConstDef(ptr.first->SonNode[0]);	
	}
	else{
		transConstTemp(ptr.first->SonNode[0]);
		transConstDef(ptr.first->SonNode[2]);	
	}
}

void transConstDef(Type ptr){
//	IDENT ArrayTemp	ConstInitTemp
	using std::string;
	using std::vector;
	using std::pair;
	using std::make_pair;
	using std::to_string;
	
	temp_for_initval.clear();
	temp_for_initarray.clear();

	transArrayTemp(ptr.first->SonNode[1]);
	string ret = transConstInitTemp(ptr.first->SonNode[2]);

	string str = ptr.first->SonNode[0].first->token.m_sID;


	if (Ident2Tid.find(str) == Ident2Tid.end()){
		Ident2Tid[str] = vector<std::pair<int,int> >();		
	}

	if (Ident2Tid[str].empty() || Ident2Tid[str].back().second != Layer){
		Ident2Tid[str].push_back(make_pair(TID, Layer));
		IdentStack.push_back(make_pair(str, Layer));
		TID++;	
	}
	else{
		return;
		string err = "redeclare variant " + str;
		yyerror(err.c_str());	

	}

	int id = Ident2Tid[str].back().first;

	if (temp_for_initarray.size() == 0){

		VarType[id] = make_pair(make_pair(id,0),NumType::m_const);

		if (opt == PrintOPT::Var)	fprintf(yyout, "var T%d\n", id);

		if (temp_for_initval.size() > 0) {
			if (temp_for_initval.back().second == NumType::m_const){
				if (opt == PrintOPT::Stmt)	fprintf(yyout, "T%d = %d\n", id, temp_for_initval.back().first.second);	
				VarType[id].first.second = temp_for_initval.back().first.second;
			}
			else{
				yyerror("you should initialize const var with const exp");
			}
			temp_for_initval.pop_back();
		}
		VarArray[id] = std::vector<int>();
	}
	else{
		VarType[id] = make_pair(make_pair(id,0),NumType::m_array_const);

		for (int i = temp_for_initarray.size()-1;i > 0;i--){
			temp_for_initarray[i-1] *= temp_for_initarray[i];
		}
		VarType[id].first.second = temp_for_initarray[0];
		if (opt == PrintOPT::Var)	fprintf(yyout, "var %d T%d\n", 8, id);
		fuckArray(temp_for_initval, id, temp_for_initarray, ret);
		VarArray[id] = temp_for_initarray;
		VarArray[id].push_back(1);
	}	

}


std::string transConstInitTemp(Type ptr){
	if (ptr.first->SonNode.size() == 0) {
		return "";
	}
	else{
		return transConstInitVal(ptr.first->SonNode[1]);	
	}
}



std::string transConstInitVal(Type ptr){
	using std::string;
	string ret;
	if (ptr.first->SonNode.size() == 1){
		int S1 = transConstExp(ptr.first->SonNode[0]);
		temp_for_initval.push_back(std::make_pair(std::make_pair(0,S1),NumType::m_const));
		ret = "*";
	}
	else if (ptr.first->SonNode.size() == 2){
		ret = "{}";			
	}
	else {
		string S2 = transConstInitVal(ptr.first->SonNode[1]);
		string S3 = transConstArrayInit(ptr.first->SonNode[2]);
		ret = "{"+S2+S3+"}";	
	}	
	return ret;
}

std::string transConstArrayInit(Type ptr){
	using std::string;

	if (ptr.first->SonNode.size() == 0) {
		return "";
	}
	else{
		string str1 = transConstArrayInit(ptr.first->SonNode[0]);	
		string str2 = transConstInitVal(ptr.first->SonNode[2]);	
		return str1+str2;
	}
}

