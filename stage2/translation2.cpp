
#include "utils2.hpp"



namespace stage2{

using namespace std;

/*
void yyerror(char const *s){
    fprintf(stderr, "%s\n", s);
	fflush(stderr);
}


FILE *yyout, *yyin;
*/

extern TreeNode* root;
int VID;
map<string, tValue> tVar;


/**
 * assume that all eeyore are translated from SysS, so that each variant can only be initialized once.
 * and it must initialized immediately(which means there can be no function def). 
 */
static set<string> buffVar;

/**
 * assum that there is no error in Eeyore, so I can initialize all array variant in f_main funcion
 * because I can not initialaze array variant outside functions. Although some variant can not aceess
 * by f_main, but if eeyore is valid, these variant cannot be visited because it starts at main and cannot
 * declare function first and then define it later.
 */
static set<string> buffArr;


string T2v(string str){
	str[0] = 'v';
	return str;
}


BackEnd option;

extern string regConvert(int id);
extern int regConvert(string str);
extern vector<regValue> regHeap; 
extern list<int> regPool;

extern void transFunctionDef(TreeNode*);
void Translation(TreeNode*);
void transInitialization(TreeNode*);
pair<bool,int> transArrayTemp(TreeNode*);
void ArrayInit(string reg1 = "s0", string reg2 = "s1");

static void transGlobalDeclaration(TreeNode*);
static void PrintDecl();

static void Init(){
// can not use x0
	for (int i = 0;i < 28;i++) {
		regHeap.push_back(regValue());
//		if (i != 0 && i != 1 && (i < 22 || i > 27)) regPool.push_back(i);
		if (i != 0 && i != 1) regPool.push_back(i);
	}
}
void Translation(TreeNode* ptr){
	buffVar.clear();
	Init();
	for (auto it = ptr->SonNode.begin(); it != ptr->SonNode.end();it++){
		//fprintf(stderr, "%d\n",it->second);
		switch (it->second){
			case Label::Declaration:
				transGlobalDeclaration(it->first);
				break;
			case Label::Initialization:
				transInitialization(it->first);
				break;
			case Label::FunctionDef:

				PrintDecl();

				transFunctionDef(it->first);

				break;
			default:
				fflush(yyout);
				fprintf(stderr,"%d\n", (int) Label::ArrayTemp);
				fprintf(stderr,"%d\n", (int) it->second);
				yyerror("translation error");
				break;		
		}
	}
}

static void PrintDecl(){
	for (auto it = buffVar.begin(); it != buffVar.end(); it++){
		auto str = *it;
		if (option == BackEnd::Tigger)
			fprintf(yyout, "%s = %d\n", T2v(str).c_str(), tVar[str].val);
		if (option == BackEnd::RISCV)
			fprintf(yyout, "  .global   %s\n\
  .section  .sdata\n\
  .align    2\n\
  .type     %s, @object\n\
  .size     %s, 4\n\
%s:\n\
  .word     %d\n", T2v(str).c_str(), T2v(str).c_str(), T2v(str).c_str(), T2v(str).c_str(), tVar[str].val);
	}
	buffVar.clear();
}


void transGlobalDeclaration(TreeNode* ptr){
//	VAR ArrayTemp SYMBOL
	auto [isArr, size] = transArrayTemp(ptr->SonNode[1].first);
	string str = ptr->SonNode[2].first->token.m_str;
	if (isArr){
		//type,size
		tVar[str] = tValue(NumType::GArr, size/4, str);
		if (option == BackEnd::Tigger)
			fprintf(yyout, "%s = malloc %d\n", T2v(str).c_str(), size);
		if (option == BackEnd::RISCV)
			fprintf(yyout, "  .comm %s, %d, 4\n", T2v(str).c_str(), size);
		buffArr.insert(str);
	}
	else{
		tVar[str] = tValue(NumType::GVar, 0, str);
		buffVar.insert(str);
	}
}



pair<bool,int> transArrayTemp(TreeNode* ptr){
	if (ptr->SonNode.size() == 0) return make_pair(false,1);
	else return make_pair(true, ptr->SonNode[0].first->token.m_int);
}
void transInitialization(TreeNode* ptr){

//printf("segment fault here\n");
	if (ptr->SonNode.size() == 3){
//		SYMBOL EQUAL NUM
		string str = ptr->SonNode[0].first->token.m_str;
		if (tVar.find(str) == tVar.end()){
			string temp = "variant "+str+" didn't declare";
			yyerror(temp.c_str());
		}
		else{
			tVar[str].val = ptr->SonNode[2].first->token.m_int;
		}
	}
	else {
//    | SYMBOL LEFT_BRACKET NUM RIGHT_BRACKET EQUAL NUM
//	printf("%d\n", ptr->SonNode.size());
		string str = ptr->SonNode[0].first->token.m_str;
		int index = ptr->SonNode[2].first->token.m_int;
		int val = ptr->SonNode[5].first->token.m_int;

		//if (val != 0) 
			tVar[str].arrVal[index/4] = val;
	}
}

void ArrayInit(string reg1, string reg2){

	regHeap[regConvert(reg1)].ident = "null";
	regHeap[regConvert(reg2)].ident = "null";

	for (auto it = buffArr.begin(); it != buffArr.end(); it++){
		auto str = *it;
		if (option == BackEnd::Tigger)
			fprintf(yyout, "loadaddr %s %s\n", T2v(str).c_str(), reg1.c_str());
		if (option == BackEnd::RISCV)
			fprintf(yyout, "  la %s, %s\n", reg1.c_str(), T2v(str).c_str());
/*
		for (int i = 0;i < tVar[str].arrVal.size();i++){
			fprintf(yyout, "%s = %d\n", reg2.c_str(), tVar[str].arrVal[i]);
			fprintf(yyout, "%s[%d] = %s\n", reg1.c_str(), i*4, reg2.c_str());
		} 
*/
		for (auto i: tVar[str].arrVal){
			if (option == BackEnd::Tigger){
				fprintf(yyout, "%s = %d\n", reg2.c_str(), i.second);
				fprintf(yyout, "%s[%d] = %s\n", reg1.c_str(), i.first*4, reg2.c_str());
			}
			if (option == BackEnd::RISCV){
				fprintf(yyout, "  li %s, %d\n", reg2.c_str(), i.second);
				fprintf(yyout, "  sw %s, %d(%s)\n", reg2.c_str(), i.first*4, reg1.c_str());			
			}
		} 
		tVar[str].arrVal.clear();
	}
	buffArr.clear();	
}

}
