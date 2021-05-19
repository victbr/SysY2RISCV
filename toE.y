%{

#include "utils.hpp"
#include <map>
#include <list>
#include <stack>
#include <vector>

extern YYSTYPE yylval;

extern "C"
{
//	void yyerror(const char*);
	int yylex(void);
}

//extern void myerror(const char*);
//extern void yyerror(const char*);

extern FILE* yyin;
extern FILE* yyout;


TreeNode* root;
extern void Translation(TreeNode*);
extern void Recycle(TreeNode*);
%}



%start		CompUnit

%type<type>	Decl ConstDecl BType ConstDef ConstInitVal VarDecl VarDef
%type<type>	InitVal FuncDef FuncFParams FuncFParam Block BlockItem Stmt 
%type<type>	Exp Cond LVal PrimaryExp Number UnaryExp UnaryOp FuncRParams 
%type<type>	MulExp AddExp RelExp EqExp LAndExp LOrExp ConstExp

%type<type>	Stmt_Other Stmt_NoElse 

%type<type>	VarTemp ConstTemp LValArrayTemp FuncFParamsList FuncRParamsTemp MulOp BlockItems RelOp
%type<type>	ArrayTemp
%type<type>	VarInitTemp
%type<type>	ArrayInit
%type<type>	ConstInitTemp
%type<type>	ConstArrayInit
%type<type>	FuncParamArray
%type<type>	FuncParamArrayTemp
%type<type>	FuncHead


%token<type>	IDENT
%token<type>	INT_CONST
%token<type>	INT SEMICOLON COMMA LEFT_BRACKET RIGHT_BRACKET EQUAL LEFT_BRACE RIGHT_BRACE CONST
%token<type>	LEFT_PARENTHESIS RIGHT_PARENTHESIS VOID
%token<type>	LEQ GEQ EQL ADD SUB NOT MUL DIV MOD LES GRT NEQ AND OR
%token<type>	WHILE BREAK CONTINUE RETURN IF ELSE


%%

CompUnit:
	Decl
	{
		root->SonNode.push_back($1);
	}
	| CompUnit Decl
	{
		root->SonNode.push_back($2);
	}
	| FuncDef
	{
		root->SonNode.push_back($1);
	}
	| CompUnit FuncDef
	{
		root->SonNode.push_back($2);
	};

Decl:
	VarDecl
	{
		using std::make_pair;
		TreeNode* node = new TreeNode();
		node->SonNode.push_back($1);
		$$ = make_pair(node, Label::Decl);
	}
	| ConstDecl
	{
		using std::make_pair;
		TreeNode* node = new TreeNode();
		node->SonNode.push_back($1);
		$$ = make_pair(node, Label::Decl);
	};

BType:
	INT
	{
		using std::make_pair;
		TreeNode* node = new TreeNode();
		$$ = make_pair(node, Label::INT);
	};

VarDecl:
	BType VarDef VarTemp SEMICOLON
	{
		using std::make_pair;
		TreeNode* node = new TreeNode();
		node->SonNode.push_back($1);
		node->SonNode.push_back($2);
		node->SonNode.push_back($3);
		node->SonNode.push_back($4);
		$$ = make_pair(node, Label::VarDecl);
	};

VarTemp:
	{
		using std::make_pair;
		TreeNode* node = new TreeNode();
		$$ = make_pair(node, Label::VarTemp);
	}
	| VarTemp COMMA VarDef
	{
		using std::make_pair;
		TreeNode* node = new TreeNode();
		node->SonNode.push_back($1);
		node->SonNode.push_back($2);
		node->SonNode.push_back($3);
		$$ = make_pair(node, Label::VarTemp);
	};

VarDef:
	IDENT ArrayTemp	VarInitTemp
	{
		using std::make_pair;
		TreeNode* node = new TreeNode();
		node->SonNode.push_back($1);
		node->SonNode.push_back($2);
		node->SonNode.push_back($3);
		$$ = make_pair(node, Label::VarDef);
	};

VarInitTemp:
	{
		using std::make_pair;
		TreeNode* node = new TreeNode();
		$$ = make_pair(node, Label::VarInitTemp);
	}
	| EQUAL InitVal
	{
		using std::make_pair;
		TreeNode* node = new TreeNode();
		node->SonNode.push_back($1);
		node->SonNode.push_back($2);
		$$ = make_pair(node, Label::VarInitTemp);
	};

ArrayTemp:
	{
		using std::make_pair;
		TreeNode* node = new TreeNode();
		$$ = make_pair(node, Label::ArrayTemp);
	}
	| ArrayTemp LEFT_BRACKET ConstExp RIGHT_BRACKET
	{
		using std::make_pair;
		TreeNode* node = new TreeNode();
		node->SonNode.push_back($1);
		node->SonNode.push_back($2);
		node->SonNode.push_back($3);
		node->SonNode.push_back($4);
		$$ = make_pair(node, Label::ArrayTemp);
	};

InitVal:
	Exp
	{
		using std::make_pair;
		TreeNode* node = new TreeNode();
		node->SonNode.push_back($1);
		$$ = make_pair(node, Label::InitVal);
	}
	| LEFT_BRACE RIGHT_BRACE
	{
		using std::make_pair;
		TreeNode* node = new TreeNode();
		node->SonNode.push_back($1);
		node->SonNode.push_back($2);
		$$ = make_pair(node, Label::InitVal);
	}
	| LEFT_BRACE InitVal ArrayInit RIGHT_BRACE
	{
		using std::make_pair;
		TreeNode* node = new TreeNode();
		node->SonNode.push_back($1);
		node->SonNode.push_back($2);
		node->SonNode.push_back($3);
		node->SonNode.push_back($4);
		$$ = make_pair(node, Label::InitVal);
	};

ArrayInit:
	{
		using std::make_pair;
		TreeNode* node = new TreeNode();
		$$ = make_pair(node, Label::ArrayInit);
	}
	| ArrayInit COMMA InitVal
	{
		using std::make_pair;
		TreeNode* node = new TreeNode();
		node->SonNode.push_back($1);
		node->SonNode.push_back($2);
		node->SonNode.push_back($3);
		$$ = make_pair(node, Label::ArrayInit);
	};




/**
 * const var declare
 */

ConstDecl:
	CONST BType ConstTemp SEMICOLON
	{
		using std::make_pair;
		TreeNode* node = new TreeNode();
		node->SonNode.push_back($1);
		node->SonNode.push_back($2);
		node->SonNode.push_back($3);
		node->SonNode.push_back($4);
		$$ = make_pair(node, Label::ConstDecl);
	};

ConstTemp:
	ConstDef
	{
		using std::make_pair;
		TreeNode* node = new TreeNode();
		node->SonNode.push_back($1);
		$$ = make_pair(node, Label::ConstTemp);
	}
	| ConstTemp COMMA ConstDef
	{
		using std::make_pair;
		TreeNode* node = new TreeNode();
		node->SonNode.push_back($1);
		node->SonNode.push_back($2);
		node->SonNode.push_back($3);
		$$ = make_pair(node, Label::ConstTemp);
	};

ConstDef:
	IDENT ArrayTemp	ConstInitTemp
	{
		using std::make_pair;
		TreeNode* node = new TreeNode();
		node->SonNode.push_back($1);
		node->SonNode.push_back($2);
		node->SonNode.push_back($3);
		$$ = make_pair(node, Label::ConstDef);
	};

ConstInitTemp:
	{
		using std::make_pair;
		TreeNode* node = new TreeNode();
		$$ = make_pair(node, Label::ConstInitTemp);
	}
	| EQUAL ConstInitVal
	{
		using std::make_pair;
		TreeNode* node = new TreeNode();
		node->SonNode.push_back($1);
		node->SonNode.push_back($2);
		$$ = make_pair(node, Label::ConstInitTemp);
	};



ConstInitVal:
	ConstExp
	{
		using std::make_pair;
		TreeNode* node = new TreeNode();
		node->SonNode.push_back($1);
		$$ = make_pair(node, Label::ConstInitVal);
	}
	| LEFT_BRACE RIGHT_BRACE
	{
		using std::make_pair;
		TreeNode* node = new TreeNode();
		node->SonNode.push_back($1);
		node->SonNode.push_back($2);
		$$ = make_pair(node, Label::ConstInitVal);
	}
	| LEFT_BRACE ConstInitVal ConstArrayInit RIGHT_BRACE
	{
		using std::make_pair;
		TreeNode* node = new TreeNode();
		node->SonNode.push_back($1);
		node->SonNode.push_back($2);
		node->SonNode.push_back($3);
		node->SonNode.push_back($4);
		$$ = make_pair(node, Label::ConstInitVal);
	};

ConstArrayInit:
	{
		using std::make_pair;
		TreeNode* node = new TreeNode();
		$$ = make_pair(node, Label::ConstArrayInit);
	}
	| ConstArrayInit COMMA ConstInitVal
	{
		using std::make_pair;
		TreeNode* node = new TreeNode();
		node->SonNode.push_back($1);
		node->SonNode.push_back($2);
		node->SonNode.push_back($3);
		$$ = make_pair(node, Label::ConstArrayInit);
	};

/**
 * const declare end
 * need to do sematic analyze
 */


/**
 * expression
 */



ConstExp:
	AddExp
	{
		using std::make_pair;
		TreeNode* node = new TreeNode();
		node->SonNode.push_back($1);
		$$ = make_pair(node, Label::ConstExp);
	};


Exp:
	AddExp
	{
		using std::make_pair;
		TreeNode* node = new TreeNode();
		node->SonNode.push_back($1);
		$$ = make_pair(node, Label::Exp);
	};

LVal:
	IDENT LValArrayTemp
	{
		using std::make_pair;
		TreeNode* node = new TreeNode();
		node->SonNode.push_back($1);
		node->SonNode.push_back($2);
		$$ = make_pair(node, Label::LVal);
	};

LValArrayTemp:
	{
		using std::make_pair;
		TreeNode* node = new TreeNode();
		$$ = make_pair(node, Label::LValArrayTemp);
	}
	| LValArrayTemp LEFT_BRACKET Exp RIGHT_BRACKET
	{
		using std::make_pair;
		TreeNode* node = new TreeNode();
		node->SonNode.push_back($1);
		node->SonNode.push_back($2);
		node->SonNode.push_back($3);
		node->SonNode.push_back($4);
		$$ = make_pair(node, Label::LValArrayTemp);
	};

PrimaryExp:
	LEFT_PARENTHESIS Exp RIGHT_PARENTHESIS 
	{
		using std::make_pair;
		TreeNode* node = new TreeNode();
		node->SonNode.push_back($1);
		node->SonNode.push_back($2);
		node->SonNode.push_back($3);
		$$ = make_pair(node, Label::PrimaryExp);	
	}
	| LVal 
	{
		using std::make_pair;
		TreeNode* node = new TreeNode();
		node->SonNode.push_back($1);
		$$ = make_pair(node, Label::PrimaryExp);
	}
	| Number
	{
		using std::make_pair;
		TreeNode* node = new TreeNode();
		node->SonNode.push_back($1);
		$$ = make_pair(node, Label::PrimaryExp);
	};

Number:
	INT_CONST
	{
		using std::make_pair;
		TreeNode* node = new TreeNode();
		node->SonNode.push_back($1);
		$$ = make_pair(node, Label::Number);
	};

UnaryExp:
	PrimaryExp
	{
		using std::make_pair;
		TreeNode* node = new TreeNode();
		node->SonNode.push_back($1);
		$$ = make_pair(node, Label::UnaryExp);	
	}
	| IDENT LEFT_PARENTHESIS FuncRParams RIGHT_PARENTHESIS
	{
		using std::make_pair;
		TreeNode* node = new TreeNode();
		node->SonNode.push_back($1);
		node->SonNode.push_back($2);
		node->SonNode.push_back($3);
		node->SonNode.push_back($4);
		$$ = make_pair(node, Label::UnaryExp);			
	}
	| UnaryOp UnaryExp
	{
		using std::make_pair;
		TreeNode* node = new TreeNode();
		node->SonNode.push_back($1);
		node->SonNode.push_back($2);
		$$ = make_pair(node, Label::UnaryExp);	
	};
	
UnaryOp:
	ADD
	{	
		using std::make_pair;
		TreeNode* node = new TreeNode();
		node->SonNode.push_back($1);
		$$ = make_pair(node, Label::UnaryOp);	
	}
	| SUB
	{
		using std::make_pair;
		TreeNode* node = new TreeNode();
		node->SonNode.push_back($1);
		$$ = make_pair(node, Label::UnaryOp);	
	}
	| NOT
	{
		using std::make_pair;
		TreeNode* node = new TreeNode();
		node->SonNode.push_back($1);
		$$ = make_pair(node, Label::UnaryOp);	
	};

FuncRParams:
	{
		using std::make_pair;
		TreeNode* node = new TreeNode();
		$$ = make_pair(node, Label::FuncRParams);
	}
	| FuncRParamsTemp
	{
		using std::make_pair;
		TreeNode* node = new TreeNode();
		node->SonNode.push_back($1);
		$$ = make_pair(node, Label::FuncRParams);
	};

FuncRParamsTemp:
	Exp
	{
		using std::make_pair;
		TreeNode* node = new TreeNode();
		node->SonNode.push_back($1);
		$$ = make_pair(node, Label::FuncRParamsTemp);			
	}	
	| FuncRParamsTemp COMMA Exp
	{
		using std::make_pair;
		TreeNode* node = new TreeNode();
		node->SonNode.push_back($1);
		node->SonNode.push_back($2);
		node->SonNode.push_back($3);
		$$ = make_pair(node, Label::FuncRParamsTemp);				
	};

MulExp:
	UnaryExp 
	{	
		using std::make_pair;
		TreeNode* node = new TreeNode();
		node->SonNode.push_back($1);
		$$ = make_pair(node, Label::MulExp);	
	}
	| MulExp MulOp UnaryExp
	{
		using std::make_pair;
		TreeNode* node = new TreeNode();
		node->SonNode.push_back($1);
		node->SonNode.push_back($2);
		node->SonNode.push_back($3);
		$$ = make_pair(node, Label::MulExp);
	};


MulOp:
	MUL
	{	
		using std::make_pair;
		TreeNode* node = new TreeNode();
		node->SonNode.push_back($1);
		$$ = make_pair(node, Label::MulOp);
	}
	| DIV
	{	
		using std::make_pair;
		TreeNode* node = new TreeNode();
		node->SonNode.push_back($1);
		$$ = make_pair(node, Label::MulOp);
	}
	| MOD
	{	
		using std::make_pair;
		TreeNode* node = new TreeNode();
		node->SonNode.push_back($1);
		$$ = make_pair(node, Label::MulOp);
	};


AddExp:
	MulExp 
	{	
		using std::make_pair;
		TreeNode* node = new TreeNode();
		node->SonNode.push_back($1);
		$$ = make_pair(node, Label::AddExp);
	}
	| AddExp UnaryOp MulExp
	{
		using std::make_pair;
		TreeNode* node = new TreeNode();
		node->SonNode.push_back($1);
		node->SonNode.push_back($2);
		node->SonNode.push_back($3);
		$$ = make_pair(node, Label::AddExp);
	};


/**
 * expression end
 */

/**
 * func def
 */



FuncDef:
	FuncHead Block
	{
		using std::make_pair;
		TreeNode* node = new TreeNode();
		node->SonNode.push_back($1);
		node->SonNode.push_back($2);
		$$ = make_pair(node, Label::FuncDef);
	};

FuncHead:
	VOID IDENT LEFT_PARENTHESIS FuncFParamsList RIGHT_PARENTHESIS
	{
		using std::make_pair;
		TreeNode* node = new TreeNode();
		node->SonNode.push_back($1);
		node->SonNode.push_back($2);
		node->SonNode.push_back($3);
		node->SonNode.push_back($4);
		node->SonNode.push_back($5);
		$$ = make_pair(node, Label::FuncHead);
	}
	| BType IDENT LEFT_PARENTHESIS FuncFParamsList RIGHT_PARENTHESIS
	{
		using std::make_pair;
		TreeNode* node = new TreeNode();
		node->SonNode.push_back($1);
		node->SonNode.push_back($2);
		node->SonNode.push_back($3);
		node->SonNode.push_back($4);
		node->SonNode.push_back($5);
		$$ = make_pair(node, Label::FuncHead);
	};	

FuncFParamsList:
	{
		using std::make_pair;
		TreeNode* node = new TreeNode();
		$$ = make_pair(node, Label::FuncFParamsList);
	}
	| FuncFParams
	{
		using std::make_pair;
		TreeNode* node = new TreeNode();
		node->SonNode.push_back($1);
		$$ = make_pair(node, Label::FuncFParamsList);
	};

FuncFParams:   
	FuncFParam
	{
		using std::make_pair;
		TreeNode* node = new TreeNode();
		node->SonNode.push_back($1);
		$$ = make_pair(node, Label::FuncFParams);
	}
	| FuncFParams COMMA FuncFParam
	{
		using std::make_pair;
		TreeNode* node = new TreeNode();
		node->SonNode.push_back($1);
		node->SonNode.push_back($2);
		node->SonNode.push_back($3);
		$$ = make_pair(node, Label::FuncFParams);
	};

FuncFParam:
	BType IDENT FuncParamArray
	{
		using std::make_pair;
		TreeNode* node = new TreeNode();
		node->SonNode.push_back($1);
		node->SonNode.push_back($2);
		node->SonNode.push_back($3);
		$$ = make_pair(node, Label::FuncFParam);
	};

FuncParamArray:
	{
		using std::make_pair;
		TreeNode* node = new TreeNode();
		$$ = make_pair(node, Label::FuncParamArray);
	}
	| LEFT_BRACKET RIGHT_BRACKET FuncParamArrayTemp
	{
		using std::make_pair;
		TreeNode* node = new TreeNode();
		node->SonNode.push_back($1);
		node->SonNode.push_back($2);
		node->SonNode.push_back($3);
		$$ = make_pair(node, Label::FuncParamArray);
	};

FuncParamArrayTemp:
	{
		using std::make_pair;
		TreeNode* node = new TreeNode();
		$$ = make_pair(node, Label::FuncParamArrayTemp);	
	}
	| FuncParamArrayTemp LEFT_BRACKET ConstExp RIGHT_BRACKET
	{
		using std::make_pair;
		TreeNode* node = new TreeNode();
		node->SonNode.push_back($1);
		node->SonNode.push_back($2);
		node->SonNode.push_back($3);
		node->SonNode.push_back($4);
		$$ = make_pair(node, Label::FuncParamArrayTemp);
	};

/**
 * func def end
 */


/**
 * block
*/


Block:
	LEFT_BRACE BlockItems RIGHT_BRACE
	{
		using std::make_pair;
		TreeNode* node = new TreeNode();
		node->SonNode.push_back($1);
		node->SonNode.push_back($2);
		node->SonNode.push_back($3);
		$$ = make_pair(node, Label::Block);
	};

BlockItems:
	{
		using std::make_pair;
		TreeNode* node = new TreeNode();
		$$ = make_pair(node, Label::BlockItems);
	}
	| BlockItems BlockItem
	{
	//	std::cout << "yacc: more items\n";
		using std::make_pair;
		TreeNode* node = new TreeNode();
		node->SonNode.push_back($1);
		node->SonNode.push_back($2);
		$$ = make_pair(node, Label::BlockItems);
	};

BlockItem:
	Decl
	{	
		using std::make_pair;
		TreeNode* node = new TreeNode();
		node->SonNode.push_back($1);
		$$ = make_pair(node, Label::BlockItem);
	} 
	| Stmt
	{
		using std::make_pair;
		TreeNode* node = new TreeNode();
		node->SonNode.push_back($1);
		$$ = make_pair(node, Label::BlockItem);
	};

Stmt:
	Stmt_Other
	{
		using std::make_pair;
		TreeNode* node = new TreeNode();
		node->SonNode.push_back($1);
		$$ = make_pair(node, Label::Stmt);
	}
	| Stmt_NoElse
	{
		using std::make_pair;
		TreeNode* node = new TreeNode();
		node->SonNode.push_back($1);
		$$ = make_pair(node, Label::Stmt);
	};

Stmt_Other:
	LVal EQUAL Exp SEMICOLON
	{
		using std::make_pair;
		TreeNode* node = new TreeNode();
		node->SonNode.push_back($1);
		node->SonNode.push_back($2);
		node->SonNode.push_back($3);
		node->SonNode.push_back($4);
		$$ = make_pair(node, Label::Stmt_Other);
	}
	| Exp SEMICOLON
	{
		using std::make_pair;
		TreeNode* node = new TreeNode();
		node->SonNode.push_back($1);
		node->SonNode.push_back($2);
		$$ = make_pair(node, Label::Stmt_Other);
	}
	| SEMICOLON
	{
		using std::make_pair;
		TreeNode* node = new TreeNode();
		node->SonNode.push_back($1);
		$$ = make_pair(node, Label::Stmt_Other);
	}
    | Block
	{
		using std::make_pair;
		TreeNode* node = new TreeNode();
		node->SonNode.push_back($1);
		$$ = make_pair(node, Label::Stmt_Other);
	}  
	| WHILE LEFT_PARENTHESIS Cond RIGHT_PARENTHESIS Stmt_Other
	{
		using std::make_pair;
		TreeNode* node = new TreeNode();
		node->SonNode.push_back($1);
		node->SonNode.push_back($2);
		node->SonNode.push_back($3);
		node->SonNode.push_back($4);
		node->SonNode.push_back($5);
		$$ = make_pair(node, Label::Stmt_Other);
	}            
	| BREAK SEMICOLON
	{
		using std::make_pair;
		TreeNode* node = new TreeNode();
		node->SonNode.push_back($1);
		node->SonNode.push_back($2);
		$$ = make_pair(node, Label::Stmt_Other);
	}        
	| CONTINUE SEMICOLON
	{
		using std::make_pair;
		TreeNode* node = new TreeNode();
		node->SonNode.push_back($1);
		node->SonNode.push_back($2);
		$$ = make_pair(node, Label::Stmt_Other);
	}            
	| RETURN SEMICOLON
	{
		using std::make_pair;
		TreeNode* node = new TreeNode();
		node->SonNode.push_back($1);
		node->SonNode.push_back($2);
		$$ = make_pair(node, Label::Stmt_Other);
	}
	| RETURN Exp SEMICOLON
	{
		using std::make_pair;
		TreeNode* node = new TreeNode();
		node->SonNode.push_back($1);
		node->SonNode.push_back($2);
		node->SonNode.push_back($3);
		$$ = make_pair(node, Label::Stmt_Other);
	}
	| IF LEFT_PARENTHESIS Cond RIGHT_PARENTHESIS Stmt_Other ELSE Stmt_Other
	{
		using std::make_pair;
		TreeNode* node = new TreeNode();
		node->SonNode.push_back($1);
		node->SonNode.push_back($2);
		node->SonNode.push_back($3);
		node->SonNode.push_back($4);
		node->SonNode.push_back($5);
		node->SonNode.push_back($6);
		node->SonNode.push_back($7);
		$$ = make_pair(node, Label::Stmt_Other);
	};

Stmt_NoElse:
	IF LEFT_PARENTHESIS Cond RIGHT_PARENTHESIS Stmt
	{
		using std::make_pair;
		TreeNode* node = new TreeNode();
		node->SonNode.push_back($1);
		node->SonNode.push_back($2);
		node->SonNode.push_back($3);
		node->SonNode.push_back($4);
		node->SonNode.push_back($5);
		$$ = make_pair(node, Label::Stmt_NoElse);		
	}
	| IF LEFT_PARENTHESIS Cond RIGHT_PARENTHESIS Stmt_Other ELSE Stmt_NoElse
	{
		using std::make_pair;
		TreeNode* node = new TreeNode();
		node->SonNode.push_back($1);
		node->SonNode.push_back($2);
		node->SonNode.push_back($3);
		node->SonNode.push_back($4);
		node->SonNode.push_back($5);
		node->SonNode.push_back($6);
		node->SonNode.push_back($7);
		$$ = make_pair(node, Label::Stmt_NoElse);
	}
	| WHILE LEFT_PARENTHESIS Cond RIGHT_PARENTHESIS Stmt_NoElse
	{
		using std::make_pair;
		TreeNode* node = new TreeNode();
		node->SonNode.push_back($1);
		node->SonNode.push_back($2);
		node->SonNode.push_back($3);
		node->SonNode.push_back($4);
		node->SonNode.push_back($5);
		$$ = make_pair(node, Label::Stmt_NoElse);
	};	

Cond:
	LOrExp
	{
		using std::make_pair;
		TreeNode* node = new TreeNode();
		node->SonNode.push_back($1);
		$$ = make_pair(node, Label::Cond);
	};

RelExp:
	AddExp
	{
		using std::make_pair;
		TreeNode* node = new TreeNode();
		node->SonNode.push_back($1);
		$$ = make_pair(node, Label::RelExp);
	}
	| RelExp RelOp AddExp
	{
		using std::make_pair;
		TreeNode* node = new TreeNode();
		node->SonNode.push_back($1);
		node->SonNode.push_back($2);
		node->SonNode.push_back($3);
		$$ = make_pair(node, Label::RelExp);
	};

RelOp:
	LES
	{
		using std::make_pair;
		TreeNode* node = new TreeNode();
		node->SonNode.push_back($1);
		$$ = make_pair(node, Label::RelOp);
	}
	| GRT
	{
		using std::make_pair;
		TreeNode* node = new TreeNode();
		node->SonNode.push_back($1);
		$$ = make_pair(node, Label::RelOp);
	}
	| LEQ
	{
		using std::make_pair;
		TreeNode* node = new TreeNode();
		node->SonNode.push_back($1);
		$$ = make_pair(node, Label::RelOp);
	}
	| GEQ
	{
		using std::make_pair;
		TreeNode* node = new TreeNode();
		node->SonNode.push_back($1);
		$$ = make_pair(node, Label::RelOp);
	};

EqExp:
	RelExp 
	{
		using std::make_pair;
		TreeNode* node = new TreeNode();
		node->SonNode.push_back($1);
		$$ = make_pair(node, Label::EqExp);
	}
	| EqExp EQL RelExp
	{
		using std::make_pair;
		TreeNode* node = new TreeNode();
		node->SonNode.push_back($1);
		node->SonNode.push_back($2);
		node->SonNode.push_back($3);
		$$ = make_pair(node, Label::EqExp);
	}
	| EqExp NEQ RelExp
	{
		using std::make_pair;
		TreeNode* node = new TreeNode();
		node->SonNode.push_back($1);
		node->SonNode.push_back($2);
		node->SonNode.push_back($3);
		$$ = make_pair(node, Label::EqExp);
	};

LAndExp:
	EqExp 
	{
		using std::make_pair;
		TreeNode* node = new TreeNode();
		node->SonNode.push_back($1);
		$$ = make_pair(node, Label::LAndExp);
	}
	| LAndExp AND EqExp
	{
		using std::make_pair;
		TreeNode* node = new TreeNode();
		node->SonNode.push_back($1);
		node->SonNode.push_back($2);
		node->SonNode.push_back($3);
		$$ = make_pair(node, Label::LAndExp);
	};
LOrExp: 
	LAndExp
	{
		using std::make_pair;
		TreeNode* node = new TreeNode();
		node->SonNode.push_back($1);
		$$ = make_pair(node, Label::LOrExp);	
	} 
	| LOrExp OR LAndExp
	{
		using std::make_pair;
		TreeNode* node = new TreeNode();
		node->SonNode.push_back($1);
		node->SonNode.push_back($2);
		node->SonNode.push_back($3);
		$$ = make_pair(node, Label::LOrExp);
	};




/**
 * block end
 */
%%

void yyerror(char const *s){
    fprintf(stderr, "%s\n", s);
	fflush(stderr);
}

/**
 * used to debug
 * @param s a character array about the debug infomation
 */ 
void myerror(char const *s){
    fprintf(stderr, "%s\n", s);
	fflush(stderr);
}
void myerror(NumType val){
//m_var, m_array_var, m_const, m_array_const, m_null, m_array_id, m_func_var, m_func_array, m_func_id};
	switch (val){
		case NumType::m_null: printf("m_null\n"); break;
		case NumType::m_var: printf("m_var\n"); break;
		case NumType::m_array_var: printf("m_array_var\n"); break;
		case NumType::m_const: printf("m_const\n"); break;
		case NumType::m_array_const: printf("m_array_const\n"); break;
		case NumType::m_array_id: printf("m_array_id\n"); break;
		case NumType::m_func_var: printf("m_func_var\n"); break;
		case NumType::m_func_array: printf("m_func_array\n"); break;
		case NumType::m_func_id: printf("m_func_id\n"); break;
		default:break;
	}
}

int main(int argc, char** args){
lastNo = 1;
lineNo = 1;
	for (int i = 1;i < argc;i++){
		if (args[i][0] == '-'){
			switch(args[i][1]){
				case 'S':
					break;
				case 'e':
					break;
				case 'o':
					i++;
					yyout = fopen(args[i],"w");
					break;
				default:
					break;
			}		
		}
		else{
			yyin = fopen(args[i],"r");
		}		
	}
	root = new TreeNode();
	yyparse();

	Translation(root);
	Recycle(root);
	return 0;
}
