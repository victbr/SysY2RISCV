#include "utils.hpp"


std::vector<Value> temp_for_initval;
std::vector<int> temp_for_initarray;


std::map<std::string, std::vector<std::pair<int,int> > > Ident2Tid; //ident, id, layer
std::map<int, Value> VarType; //tid, Value( id value/index/0, type )
std::map<int, std::vector<int> > VarArray;
std::vector<std::pair<std::string, int> > IdentStack;//ident, layer

std::map<int, std::map<int, int> > ConstArray;
//maybe there is a constarray

int Layer, TID, tempID, LID;
int lineNo,lastNo;
PrintOPT opt;
/**
 * because block is not a function, but ident can cover.
 * so  
 */

extern std::map<std::string, std::map<std::string,int> >IdentFunc; //global info, need to clean up


extern void addLibFunc();
extern Value transExp(Type ptr);
extern int transConstExp(Type ptr);
extern void transFuncDef(Type type);
extern void transConstDecl(Type type);

void fuckArray(std::vector<Value>& value, int tid, std::vector<int>& arraySize, std::string str);
int transArrayTemp(Type ptr);
std::string transInitVal(Type ptr);
std::string transArrayInit(Type ptr);
std::string transVarInitTemp(Type ptr);
void transVarDef(Type ptr);
void transVarTemp(Type ptr);
void transVarDecl(Type ptr);
void transDecl(Type ptr);

void Translation(TreeNode* ptr);



void fuckArray(std::vector<Value>& value, int tid, std::vector<int>& arraySize, std::string str){
	int layer = -1;
	bool flag = true;
	std::vector<Fuck> m_stack = std::vector<Fuck>();
	int cnt = 0, index = 0, bottom = arraySize.back();
	bool noEle = true;
	ConstArray[tid] = std::map<int,int>();

	for (int i = 0;i < str.length();i++){
		if (str[i] == '{'){
			if (flag) {
				noEle = true;
				layer++; 
				if (layer < arraySize.size()-1)
					m_stack.push_back(Fuck::top);
				else {
					flag = false;
					m_stack.push_back(Fuck::bottom);
				}
			}
			else if (m_stack.back() == Fuck::single){
				yyerror("array init error single");
				return;
			}
			else if (m_stack.back() == Fuck::bottom){
				noEle = false;
				m_stack.push_back(Fuck::single);
			} else {
				noEle = true;
				if ( (index) % bottom != 0) {
printf("%d %d\n",index, bottom);
					yyerror("array init error bottom != 0");
					return;				
				}
				m_stack.push_back(Fuck::bottom);
			}
		}
		else if (str[i] == '}'){
			if (m_stack.back() == Fuck::top){
				while (noEle || index % arraySize[layer] != 0){
					noEle = false;					
					if (opt == PrintOPT::Stmt)	fprintf(yyout, "T%d[%d] = 0\n", tid, index*4);
					ConstArray[tid][index*4] = 0;
					index++;
				}
				flag = true;
				layer--;			
			}
			else if (m_stack.back() == Fuck::single){
				if (opt == PrintOPT::Stmt)	fprintf(yyout, "T%d[%d] = 0\n", tid, index*4);
				ConstArray[tid][index*4] = 0;
				index++;
			} else{
				while (noEle || index % bottom != 0){	
					noEle = false;				
					if (opt == PrintOPT::Stmt)	fprintf(yyout, "T%d[%d] = 0\n", tid, index*4);
					ConstArray[tid][index*4] = 0;
					index++;			
				}
				flag = false;			
			}
			m_stack.pop_back();
		}
		else{
			flag = false;
			noEle = false;
			if (value[cnt].second == NumType::m_const){		
				if (opt == PrintOPT::Stmt)	fprintf(yyout, "T%d[%d] = %d\n", tid, index*4, value[cnt].first.second);
				ConstArray[tid][index*4] = value[cnt].first.second;
			}
			else {
				if (opt == PrintOPT::Stmt)	fprintf(yyout, "T%d[%d] = t%d\n", tid, index*4, value[cnt].first.first);
			}
			cnt++;
			index++;
		}
	}
	return;
}


int transArrayTemp(Type ptr){
	if (ptr.first->SonNode.size() == 0) return 1;
	int val1 = transArrayTemp(ptr.first->SonNode[0]);
	int val2 = transConstExp(ptr.first->SonNode[2]);
	temp_for_initarray.push_back(val2);
	return val1*val2;
}

std::string transInitVal(Type ptr){
	using std::string;

	string ret;

	int temp = ptr.first->SonNode.size();
	if (temp == 1){
		Value val = transExp(ptr.first->SonNode[0]);
		temp_for_initval.push_back(val);
		ret = "*";
	}
	else if (temp == 2){
		ret = "{}";		
	}
	else{
//LEFT_BRACE InitVal ArrayInit RIGHT_BRACE
		string t1 = transInitVal(ptr.first->SonNode[1]);
		string t2 = transArrayInit(ptr.first->SonNode[2]);
		ret = "{"+t1+t2+"}";		
	}
	return ret;
}

std::string transArrayInit(Type ptr){
	using std::string;
	string t1,t2;
//	| ArrayInit COMMA InitVal
	
	if (ptr.first->SonNode.size() == 0)	return "";
	t1 = transArrayInit(ptr.first->SonNode[0]);
	t2 = transInitVal(ptr.first->SonNode[2]);
	return t1+t2;
}

std::string transVarInitTemp(Type ptr){
	using std::string;
	if (ptr.first->SonNode.size() == 0) return "";
	return transInitVal(ptr.first->SonNode[1]);		
}


void transVarDef(Type ptr){
	using std::string;
	using std::vector;
	using std::pair;
	using std::make_pair;
	using std::to_string;

//	IDENT ArrayTemp	VarInitTemp
	
	temp_for_initval.clear();
	temp_for_initarray.clear();

	transArrayTemp(ptr.first->SonNode[1]);


	string ret = transVarInitTemp(ptr.first->SonNode[2]);


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
		string err = "redeclare variant " + str + " in layer " + to_string(Layer);
		yyerror(err.c_str());	
	}

	int id = Ident2Tid[str].back().first;

	if (temp_for_initarray.size() == 0){

		VarType[id] = make_pair(make_pair(id,0),NumType::m_var);

		if (opt == PrintOPT::Var)	fprintf(yyout, "var T%d\n", id);

		if (temp_for_initval.size() > 0) {
			if (temp_for_initval.back().second == NumType::m_const){
				if (opt == PrintOPT::Stmt)	fprintf(yyout, "T%d = %d\n", id, temp_for_initval.back().first.second);	
				VarType[id].first.second = temp_for_initval.back().first.second;
			}
			else{
				if (opt == PrintOPT::Stmt)	fprintf(yyout, "T%d = t%d\n", id, temp_for_initval.back().first.first);
				VarType[id].first.second = temp_for_initval.back().first.second;
			}
			temp_for_initval.pop_back();
		}
		VarArray[id] = std::vector<int>();
	}
	else{
		VarType[id] = make_pair(make_pair(id,0),NumType::m_array_var);

		for (int i = temp_for_initarray.size()-1;i > 0;i--){
			temp_for_initarray[i-1] *= temp_for_initarray[i];			
		}
		VarType[id].first.second = temp_for_initarray[0];

		if (opt == PrintOPT::Var)	fprintf(yyout, "var %d T%d\n", temp_for_initarray[0] * 4, id);
		fuckArray(temp_for_initval, id, temp_for_initarray, ret);
		VarArray[id] = temp_for_initarray;
		VarArray[id].push_back(1);
	}	
}

void transVarTemp(Type ptr){
	if (ptr.first->SonNode.size() == 0) return;
	
	for (auto it : ptr.first->SonNode){
		switch (it.second){
			case Label::VarTemp:
				transVarTemp(it);
				break;

			case Label::COMMA:
				break;
			case Label::VarDef:
				transVarDef(it);
				break;
			default:
				break;
		}	
	}	
}

void transVarDecl(Type ptr){
	if (ptr.first->SonNode.size() != 4){
		yyerror("VarDecl error");
	}
	for (auto it : ptr.first->SonNode){
		switch (it.second){
			case Label::INT:
				break;
			case Label::VarDef:
				transVarDef(it);				
				break;
			case Label::VarTemp:
				transVarTemp(it);
				break;
			case Label::SEMICOLON:
				break;
			default:
				break;
		}
	}
	
}


void transDecl(Type ptr){
	if (ptr.first->SonNode.size() != 1) {
		yyerror("Decl error");	
	}
	auto it = ptr.first->SonNode.back();
	switch (it.second){
		case Label::VarDecl:
			transVarDecl(it);
			break;
		case Label::ConstDecl:
			transConstDecl(it);
			break;
		default:
			yyerror("decl parser error");
			break;	
	}
}





//count version



//count version end

void Translation(TreeNode* ptr){
	addLibFunc();// because funcret will never be clean up, so it is find to do only once
	Ident2Tid.clear();
	IdentStack.clear();
	VarType.clear();
	VarArray.clear();
	Layer = 0;
	TID = 0;
	tempID = 0;
	LID = 0;
	IdentFunc.clear();
	for (auto it: ptr->SonNode){

		switch (it.second){
			case Label::Decl:
				opt = PrintOPT::Var;
				transDecl(it);
				break;
			case Label::FuncDef:
				opt = PrintOPT::Nothing;
				transFuncDef(it);				
				break;
			default:
				yyerror("CompUnit parser error");
				break;		
		}
	}


	Ident2Tid.clear();
	IdentStack.clear();
	VarType.clear();
	VarArray.clear();
	Layer = 0;
	TID = 0;
	LID = 0;
	tempID = 0;
	IdentFunc.clear();

	int oldTID, oldtempID, oldLID;

	for (auto it: ptr->SonNode){
		switch (it.second){
			case Label::Decl:
				opt = PrintOPT::Stmt;
				transDecl(it);
				break;
			case Label::FuncDef:
				{				
					auto oldIdentFunc = IdentFunc;
					oldTID = TID;
					oldtempID = tempID;
					oldLID = LID;

					opt = PrintOPT::Var;
					transFuncDef(it);

					IdentFunc = oldIdentFunc;
					TID = oldTID;
					tempID = oldtempID;
					LID = oldLID;

					opt = PrintOPT::Stmt;
					transFuncDef(it);
				}
				break;
			default:
				yyerror("CompUnit parser error");
				break;		
		}
	}
}

