#ifndef __UTILS_HPP__
#define __UTILS_HPP__


#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <string>
#include <memory>
#include <vector>
#include <list>
#include <map>


enum class BackEnd{Null, Tigger, Eeyore, RISCV};

enum class NumType{m_var, m_array_var, m_const, m_array_const, m_null, m_array_id, m_func_var, m_func_array, m_func_id};
enum class Fuck{top,bottom,single};
enum class Label{IDENT, INT_CONST, INT, SEMICOLON, COMMA, LEFT_BRACKET, RIGHT_BRACKET, EQUAL, LEFT_BRACE, RIGHT_BRACE, CONST,
				LEFT_PARENTHESIS, RIGHT_PARENTHESIS, VOID, LEQ, GEQ, EQL, ADD, SUB, NOT, MUL, DIV, MOD, LES, GRT, NEQ, AND, OR,
				WHILE, BREAK, CONTINUE, RETURN, IF, ELSE,
				Decl, ConstDecl, BType, ConstDef, ConstInitVal, VarDecl, VarDef,
				InitVal, FuncDef, FuncFParams, FuncFParam, Block, BlockItem, Stmt, 
				Exp, Cond, LVal, PrimaryExp, Number, UnaryExp, UnaryOp, FuncRParams, 
				MulExp, AddExp, RelExp, EqExp, LAndExp, LOrExp, ConstExp,
				Stmt_Other, Stmt_Else, Stmt_NoElse, 
				VarTemp, ConstTemp, LValArrayTemp, FuncFParamsList, FuncRParamsTemp, MulOp, BlockItems, RelOp,
				ArrayTemp,
				VarInitTemp,
				ArrayInit,
				ConstInitTemp,
				ConstArrayInit,
				FuncParamArray,
				FuncParamArrayTemp,
				FuncHead};
struct Token{
	std::string m_sID;
	int m_nINT;
	char m_cOP;
};


struct TreeNode{
	std::vector<std::pair<TreeNode*, Label> > SonNode;
	Token token;
	int lineno;
	TreeNode(){
		using std::vector;
		SonNode = vector<std::pair<TreeNode*, Label> >();
	}

};
typedef std::pair<TreeNode*, Label> Type;
typedef std::pair<std::pair<int,int>, NumType> Value;
typedef std::pair<NumType, int> Tuple;

extern TreeNode* root;

struct mTYPE{
	Type type;
};

#define YYSTYPE mTYPE
#define mydebug(...) //fprintf(__VA_ARGS__); fflush(stderr); 

const int myErrorLen = 100;

enum class PrintOPT{Nothing, Var, Stmt, Both};

extern PrintOPT opt;

extern void yyerror(const char*);
extern void myerror(const char*);	
extern void myerror(NumType); 

extern std::string myConvert(Value val);
extern int lineNo, lastNo;
extern FILE* yyin, *yyout;
#endif
