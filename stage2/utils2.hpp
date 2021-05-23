#ifndef __UTILS2_HPP__
#define __UTILS2_HPP__


#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <string>
#include <memory>
#include <vector>
#include <list>
#include <map>
#include <set>


namespace stage2{

enum class BackEnd{Null, Tigger, Eeyore, RISCV};
enum class Label{VAR, EQUAL, LEFT_BRACKET, RIGHT_BRACKET, GOTO, SYMBOL, LABEL,
			FUNCTION, END, CALL, RETURN, PARAM, IF, COLON, NUM, OP, LOGICOP, NL,
			Program, Declaration, Initialization, FunctionDef, FunctionHeader,
			Statements, FunctionEnd, Statement, Expression, RightValue, BinOp,
			ArrayTemp};

enum class NumType{Num, GVar, GArr, Var, Arr, Null, FVar};

enum class PrintType{On, Off};


struct Token{
	std::string m_str;
	int m_int;
};

struct regValue{
	std::string ident;
	int index;
	bool dirty;
};

struct tValue{
	NumType type;
	bool onStack;
	std::string ident;

// below three may be useless

	//std::vector<bool> arrStack;
	std::map<int,int> arrVal;
	//std::vector<int> arrReg;

	int val, addr, reg;//for int val is value, for arr is size
	//tValue pointer ? 
	tValue(){
		type = 	NumType::Null;
		onStack = false;
		ident = "temp";
		//arrStack = std::vector<bool>();
		arrVal = std::map<int,int>();
		val = addr = reg = 0;
	}
	tValue(NumType type, int val, std::string str):type(type),val(val), ident(str){
		if (type == NumType::GArr /*|| type == NumType::Arr*/){
			//arrStack = std::vector<bool>(val,false);
			arrVal = std::map<int,int>();
			//arrReg = std::vector<int>(val,0);
			onStack = false;
		}
		else{
			//arrStack = std::vector<bool>();
			arrVal = std::map<int,int>();
			//arrReg = std::vector<int>();
			onStack = false;
		}
		addr = reg = 0;
	}
	void FtValue(NumType type, int val, std::string str){
		this->type = type; this->val = val;
		this->ident = str;
		if (type == NumType::GArr || type == NumType::Arr){
			//arrStack = std::vector<bool>(val,false);
			//arrReg = std::vector<int>(val,0);
			onStack = false;
		}
		else{
			//arrStack = std::vector<bool>();
			//arrReg = std::vector<int>();
			onStack = false;
		}
		arrVal = std::map<int,int>();
		addr = reg = 0;	
	}
};


struct TreeNode{
	std::vector<std::pair<TreeNode*, Label> > SonNode;
	Token token;
	TreeNode(){
		using std::vector;
		SonNode = vector<std::pair<TreeNode*, Label> >();
	}

};
typedef std::pair<TreeNode*, Label> Type;


extern TreeNode* root;
extern BackEnd option;
extern void Recycle(TreeNode*);
extern void Translation(TreeNode*);

struct mTYPE{
	Type type;
};

#define YYSTYPE mTYPE
#define mydebug(...) //fprintf(__VA_ARGS__); fflush(stderr); 

const int myErrorLen = 100;

extern void yyerror(const char*);
extern void myerror(const char*);	
extern void myerror(NumType); 

extern FILE* yyin, *yyout;

}
#endif
