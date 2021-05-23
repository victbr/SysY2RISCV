#include "utils.hpp"
#include "stage2/utils2.hpp"

// maybe need a hash code to check if func call match 


extern std::map<std::string, std::vector<std::pair<int,int> > > Ident2Tid; //ident, id, layer
extern std::map<int, Value> VarType; //tid, Value( id value/index/0, type )
extern std::map<int, std::vector<int> > VarArray;
extern std::vector<std::pair<std::string, int> > IdentStack;//ident, layer

std::map<std::string, std::map<std::string,int> >IdentFunc; //ident, name, VarID
std::map<int, Value> FuncVarType; //tid, Value( id value/index/0, type )
std::map<int, std::vector<int> > FuncVarArray;
std::map<std::string, bool> FuncRet; //didn't clean because it change with IdentFunc. Can not use alone

static std::map<std::string,int> temp_for_func_param;

static int FuncVarID;
extern int Layer, TID;


extern void transBlock(Type,int,int);
extern int transConstExp(Type);

typedef std::vector<int> ArrayParam;

void transFuncDef(Type);
std::string transFuncHead(Type);
void transFuncFParamsList(Type);
void transFuncFParams(Type);
std::pair<std::string, int> transFuncFParam(Type);
void transFuncParamArray(Type, ArrayParam &);
void transFuncParamArrayTemp(Type, ArrayParam &);


/*
note: 
because func param store in different place, 
so for each param, need to check layer first, if layer > 0, use it,
else check Funcparam, and then check if there exist layer 0 param
*/

void transFuncDef(Type ptr){
	using std::string;

	//FuncHead Block
	FuncVarID = 0;
	FuncVarType.clear();
	FuncVarArray.clear();

	string str = transFuncHead(ptr.first->SonNode[0]);

	transBlock(ptr.first->SonNode[1],-1,-1);
	
	if (FuncRet[str]) {
		if (opt == PrintOPT::Stmt)
			fprintf(yyout, "return 0\n");	
	}
	else{
		if (opt == PrintOPT::Stmt)
			fprintf(yyout, "return\n");	
	}

	if (opt == PrintOPT::Stmt)	fprintf(yyout, "end f_%s\n", str.c_str());
}


std::string transFuncHead(Type ptr){

	using std::string;
	using std::make_pair;
	
	string S2 = ptr.first->SonNode[1].first->token.m_sID;

	//VOID IDENT LEFT_PARENTHESIS FuncFParamsList RIGHT_PARENTHESIS
	if (IdentFunc.find(S2) == IdentFunc.end()){
		FuncRet[S2] = !(ptr.first->SonNode[0].second == Label::VOID);
	}
	else{
		string err = "function " + S2 + " redeclare";
		yyerror(err.c_str());	
	}

	temp_for_func_param.clear();

	transFuncFParamsList(ptr.first->SonNode[3]);

	//trans var from func to global

	IdentFunc[S2] = temp_for_func_param;
	for (auto it = temp_for_func_param.begin();it != temp_for_func_param.end();it++){
		Ident2Tid[it->first].push_back(make_pair(TID, Layer+1));
		auto tmp = FuncVarType[it->second].second == NumType::m_var ? NumType::m_func_var : NumType::m_func_array;
	
		VarType[TID] = make_pair(make_pair(it->second,it->second) ,tmp);
		VarArray[TID] = FuncVarArray[it->second];
		IdentStack.push_back(make_pair(it->first,Layer+1));
		TID++;
	}
	if (opt == PrintOPT::Var)	
		fprintf(yyout, "f_%s [%lu]\n", S2.c_str(), temp_for_func_param.size());
	return S2;
}	

void transFuncFParamsList(Type ptr){
	if (ptr.first->SonNode.size() == 0){
		return;		
	}
	else {
		transFuncFParams(ptr.first->SonNode[0]);
	}
}

void transFuncFParams(Type ptr){
	if (ptr.first->SonNode[0].second == Label::FuncFParam){
		auto temp = transFuncFParam(ptr.first->SonNode[0]);
		temp_for_func_param.insert(temp);
	}
	else{
		transFuncFParams(ptr.first->SonNode[0]);
		auto temp = transFuncFParam(ptr.first->SonNode[2]);
		temp_for_func_param.insert(temp);		
	}
}

std::pair<std::string, int> transFuncFParam(Type ptr){
	using std::vector;
	using std::make_pair;

//	BType IDENT FuncParamArray
	auto S2 = ptr.first->SonNode[1].first->token.m_sID;
	
	ArrayParam temp = vector<int>();
	transFuncParamArray(ptr.first->SonNode[2], temp);
	auto ret = make_pair(S2, FuncVarID);

	if (temp.size() > 0){
		temp.push_back(1);
		for (int i = temp.size()-1;i > 0;i--){
			temp[i-1] *= temp[i];
		}
		FuncVarType[FuncVarID] = make_pair(make_pair(FuncVarID, temp[1]) , NumType::m_array_var);
	}
	else{
		FuncVarType[FuncVarID] = make_pair(make_pair(FuncVarID, 0) , NumType::m_var);
	}
	FuncVarArray[FuncVarID] = temp;
	FuncVarID++;
	//S3 is used to update vararray
	return ret;
}

void transFuncParamArray(Type ptr, ArrayParam & arr){
	using std::string;
	if (ptr.first->SonNode.size() == 0){
				
	}
	else{
		arr.push_back(0);
		transFuncParamArrayTemp(ptr.first->SonNode[2], arr);
	}

}

void transFuncParamArrayTemp(Type ptr, ArrayParam & arr){

	if (ptr.first->SonNode.size() == 0){
	}
	else{
		transFuncParamArrayTemp(ptr.first->SonNode[0], arr);
		auto S3 = transConstExp(ptr.first->SonNode[2]);
		arr.push_back(S3);	
	}
}
