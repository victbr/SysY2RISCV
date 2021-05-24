

#include "utils2.hpp"

namespace stage2{


using namespace std;



int stackPos;
extern map<string, tValue> tVar;
map<string, tValue> pVar;



extern vector<regValue> regHeap; 
extern list<int> regPool;

void transFunctionDef(TreeNode*);
pair<string,int> transFunctionHeader(TreeNode*);
void transFunctionEnd(TreeNode*);


int func_stk;
int transStatements(TreeNode*, PrintType);
int transStatement(TreeNode*, PrintType);
int transDeclaration(TreeNode*, PrintType);

int paramCount;// use for parameters, count the number of register a_

extern string regConvert(int id);
extern int regConvert(string str);
extern int calleeSave();
extern void calleeRecover();

extern int transExpression(TreeNode*, PrintType);
extern void ArrayInit(string reg1 = "s0", string reg2 = "s1");
extern pair<bool,int> transArrayTemp(TreeNode*);
/**
 * because there is no reference in SysS, so when param int, it can only 
 * means param value, not address. So I can only malloc space for array.
 */
void transFunctionDef(TreeNode* ptr){
	stackPos = 0;
	pVar.clear();
//FunctionHeader Statements FunctionEnd

	auto [ident, param] = transFunctionHeader(ptr->SonNode[0].first);

//printf("param %d\n", param);

	
	auto oldHeap = regHeap;
	auto oldPool = regPool;
	paramCount = 0;

	for (int i = 0;i < param;i++){
		string tmp = "p"+ to_string(i);
		//printf("%s\n", tmp.c_str());
		pVar[tmp] = tValue(NumType::FVar, 0, tmp);
		pVar[tmp].reg = regConvert("a"+to_string(i));
		regHeap[regConvert("a"+to_string(i))].ident = tmp;
		regHeap[regConvert("a"+to_string(i))].dirty = false;
	}

	transStatements(ptr->SonNode[1].first, PrintType::Off);

//printf("????\n");
//stackPos = 0;
	int stackSize = stackPos/4;
//	stackSize += calleeSave(); 
//	stackSize += transStatements(ptr->SonNode[1].first, PrintType::Off);
	if (option == BackEnd::Tigger)
		fprintf(yyout, "%s[%d][%d]\n", ident.c_str(), param, stackSize);
	if (option == BackEnd::RISCV){
		func_stk = (stackSize / 4 + 1) * 16;
		fprintf(yyout, "  .text\n\
  .align  2\n\
  .global %s\n\
  .type   %s, @function\n\
%s:\n\
  addi    sp, sp, -%d\n\
  sw      ra, %d(sp)\n", ident.substr(2).c_str(), ident.substr(2).c_str(), ident.substr(2).c_str(), func_stk, func_stk-4);
	}
	paramCount = 0;
	stackPos = 0;
	regHeap = oldHeap;
	regPool = oldPool;
	pVar.clear();

	for (int i = 0;i < param;i++){
		string tmp = "p"+ to_string(i);
		//printf("%s\n", tmp.c_str());
		pVar[tmp] = tValue(NumType::FVar, 0, tmp);
		pVar[tmp].reg = regConvert("a"+to_string(i));
		regHeap[regConvert("a"+to_string(i))].ident = tmp;
		regHeap[regConvert("a"+to_string(i))].dirty = false;
	}
	if (ident == "f_main") ArrayInit();

	transStatements(ptr->SonNode[1].first, PrintType::On);
//	transFunctionEnd(ptr->SonNode[2].first);
//	calleeRecover();
	if (option == BackEnd::Tigger)
		fprintf(yyout, "return\nend %s\n", ident.c_str());
	if (option == BackEnd::RISCV){
		fprintf(yyout, "  lw ra, %d(sp)\n\
  addi sp, sp, %d\n\
  ret\n", func_stk-4, func_stk);
		fprintf(yyout, "  .size   %s, .-%s\n", ident.substr(2).c_str(), ident.substr(2).c_str());
	}
}

pair<string,int> transFunctionHeader(TreeNode* ptr){
	string str = ptr->SonNode[0].first->token.m_str;
	int num = ptr->SonNode[2].first->token.m_int;
	return make_pair(str,num);
}
int transStatements(TreeNode* ptr, PrintType type){
	if (ptr->SonNode.size() == 0) {
		return 0;	
	}
	else {
		//Statements Statement
		int ret = transStatements(ptr->SonNode[0].first, type);
		ret += transStatement(ptr->SonNode[1].first, type);
		return ret;
	}
}
int transStatement(TreeNode* ptr, PrintType type){
	int ret = 0;
	if (ptr->SonNode[0].second == Label::Expression){
		//expression
		ret = transExpression(ptr->SonNode[0].first, type);
	}
	else{
		// declaration
		transDeclaration(ptr->SonNode[0].first, type);
	}
}

void transFunctionEnd(TreeNode* ptr){
//END FUNCTION
	//useless
	return;
}

int transDeclaration(TreeNode* ptr, PrintType type){

// assume every things are down in calc version
// here need to allocate stack position, cannot skip; 
	//	VAR ArrayTemp SYMBOL

	string str = ptr->SonNode[2].first->token.m_str;
	auto [isArr, size] = transArrayTemp(ptr->SonNode[1].first);
	int ret = 0;
	if (isArr){
		pVar[str] = tValue();
		pVar[str].FtValue(NumType::Arr, size/4, str);
		pVar[str].onStack = true;
		pVar[str].addr = stackPos;
		stackPos += size;
		ret = size/4;
	}
	else{
		pVar[str] = tValue(NumType::Var, 0, str);
		ret = 0;
	}	
	return ret;
}
}
