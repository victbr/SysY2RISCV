
%{

#include "utils.hpp"
#include "toE.tab.hpp"

extern "C"
{
	int yylex(void);
	int yywrap(void);
}

//extern void yyerror(const char*);
//extern void myerror(const char*);

%}


Ignore			(\\\n)*


nonDigit		([_A-Za-z])

digit			([0-9])

nonZeroDigit	([1-9])

identifier		({nonDigit}({Ignore}({digit}|{nonDigit}))*)

decConst		({nonZeroDigit}({Ignore}{digit})*)

octConst		("0"({Ignore}[0-7])*)

hexConst		(("0"{Ignore}"x"|"0"{Ignore}"X")({Ignore}[0-9a-f])+)

Const			({decConst}|{octConst}|{hexConst})

%%


"/"{Ignore}"/"(.{Ignore})*	{lineNo = lastNo;int len = strlen(yytext);for (int i = 0;i < len;i++) lastNo += yytext[i] == '\n';}

"/"{Ignore}"*"{Ignore}(([^*]{Ignore})*([^/]{Ignore})*[^*/]{Ignore})*([^*]{Ignore})*([^/]{Ignore})*"*"{Ignore}"/"	{lineNo = lastNo;int len = strlen(yytext);for (int i = 0;i < len;i++) lastNo += yytext[i] == '\n';}

[ \t\n\r]|{Ignore}			{lineNo = lastNo;int len = strlen(yytext);for (int i = 0;i < len;i++) lastNo += yytext[i] == '\n';}

\[					{
						lineNo = lastNo;int len = strlen(yytext);for (int i = 0;i < len;i++) lastNo += yytext[i] == '\n';
						using std::make_pair;
						TreeNode* node = new TreeNode();
						node->lineno = lineNo;
						yylval.type = make_pair(node, Label::LEFT_BRACKET);
						return LEFT_BRACKET;
					}

\]					{
lineNo = lastNo;int len = strlen(yytext);for (int i = 0;i < len;i++) lastNo += yytext[i] == '\n';
						using std::make_pair;
						TreeNode* node = new TreeNode();
						node->lineno = lineNo;
						yylval.type = make_pair(node, Label::RIGHT_BRACKET);
						return RIGHT_BRACKET;
					}

\{					{
lineNo = lastNo;int len = strlen(yytext);for (int i = 0;i < len;i++) lastNo += yytext[i] == '\n';
						using std::make_pair;
						TreeNode* node = new TreeNode();
						node->lineno = lineNo;
						yylval.type = make_pair(node, Label::LEFT_BRACE);
						return LEFT_BRACE;
					}

\}					{
lineNo = lastNo;int len = strlen(yytext);for (int i = 0;i < len;i++) lastNo += yytext[i] == '\n';
						using std::make_pair;
						TreeNode* node = new TreeNode();
						node->lineno = lineNo;
						yylval.type = make_pair(node, Label::RIGHT_BRACE);
						return RIGHT_BRACE;
					}

\(					{
lineNo = lastNo;int len = strlen(yytext);for (int i = 0;i < len;i++) lastNo += yytext[i] == '\n';
						using std::make_pair;
						TreeNode* node = new TreeNode();
						node->lineno = lineNo;
						yylval.type = make_pair(node, Label::LEFT_PARENTHESIS);
						return LEFT_PARENTHESIS;
					}

\)					{
lineNo = lastNo;int len = strlen(yytext);for (int i = 0;i < len;i++) lastNo += yytext[i] == '\n';
						using std::make_pair;
						TreeNode* node = new TreeNode();
						node->lineno = lineNo;
						yylval.type = make_pair(node, Label::RIGHT_PARENTHESIS);
						return RIGHT_PARENTHESIS;
					}

\<{Ignore}=					{
lineNo = lastNo;int len = strlen(yytext);for (int i = 0;i < len;i++) lastNo += yytext[i] == '\n';
						using std::make_pair;
						TreeNode* node = new TreeNode();
						node->lineno = lineNo;
						yylval.type = make_pair(node, Label::LEQ);
						return LEQ;
					}

\>{Ignore}=					{
lineNo = lastNo;int len = strlen(yytext);for (int i = 0;i < len;i++) lastNo += yytext[i] == '\n';
						using std::make_pair;
						TreeNode* node = new TreeNode();
						node->lineno = lineNo;
						yylval.type = make_pair(node, Label::GEQ);
						return GEQ;
					}

={Ignore}=					{
lineNo = lastNo;int len = strlen(yytext);for (int i = 0;i < len;i++) lastNo += yytext[i] == '\n';
						using std::make_pair;
						TreeNode* node = new TreeNode();
						node->lineno = lineNo;
						yylval.type = make_pair(node, Label::EQL);
						return EQL;
					}

=					{
lineNo = lastNo;int len = strlen(yytext);for (int i = 0;i < len;i++) lastNo += yytext[i] == '\n';
						using std::make_pair;
						TreeNode* node = new TreeNode();
						node->lineno = lineNo;
						yylval.type = make_pair(node, Label::EQUAL);
						node->token.m_cOP = '=';
						return EQUAL;
					}

\+					{
lineNo = lastNo;int len = strlen(yytext);for (int i = 0;i < len;i++) lastNo += yytext[i] == '\n';
						using std::make_pair;
						TreeNode* node = new TreeNode();
						node->lineno = lineNo;
						yylval.type = make_pair(node, Label::ADD);
						node->token.m_cOP = '+';
						return ADD;
					}

-					{
lineNo = lastNo;int len = strlen(yytext);for (int i = 0;i < len;i++) lastNo += yytext[i] == '\n';
						using std::make_pair;
						TreeNode* node = new TreeNode();
						node->lineno = lineNo;
						node->token.m_cOP = '-';
						yylval.type = make_pair(node, Label::SUB);
						return SUB;
					}

!					{
lineNo = lastNo;int len = strlen(yytext);for (int i = 0;i < len;i++) lastNo += yytext[i] == '\n';
						using std::make_pair;
						TreeNode* node = new TreeNode();
						node->lineno = lineNo;
						node->token.m_cOP = '!';
						yylval.type = make_pair(node, Label::NOT);
						return NOT;
					}

\*					{
lineNo = lastNo;int len = strlen(yytext);for (int i = 0;i < len;i++) lastNo += yytext[i] == '\n';
						using std::make_pair;
						TreeNode* node = new TreeNode();
						node->lineno = lineNo;
						node->token.m_cOP = '*';
						yylval.type = make_pair(node, Label::MUL);
						return MUL;
					}

\/					{
lineNo = lastNo;int len = strlen(yytext);for (int i = 0;i < len;i++) lastNo += yytext[i] == '\n';
						using std::make_pair;
						TreeNode* node = new TreeNode();
						node->lineno = lineNo;
						node->token.m_cOP = '/';
						yylval.type = make_pair(node, Label::DIV);
						return DIV;
					}

\%					{
lineNo = lastNo;int len = strlen(yytext);for (int i = 0;i < len;i++) lastNo += yytext[i] == '\n';
						using std::make_pair;
						TreeNode* node = new TreeNode();
						node->lineno = lineNo;
						node->token.m_cOP = '%';
						yylval.type = make_pair(node, Label::MOD);
						return MOD;
					}

\<					{
lineNo = lastNo;int len = strlen(yytext);for (int i = 0;i < len;i++) lastNo += yytext[i] == '\n';
						using std::make_pair;
						TreeNode* node = new TreeNode();
						node->lineno = lineNo;
						yylval.type = make_pair(node, Label::LES);
						return LES;
					}

\>					{
lineNo = lastNo;int len = strlen(yytext);for (int i = 0;i < len;i++) lastNo += yytext[i] == '\n';
						using std::make_pair;
						TreeNode* node = new TreeNode();
						node->lineno = lineNo;
						yylval.type = make_pair(node, Label::GRT);
						return GRT;
					}

!{Ignore}=					{
lineNo = lastNo;int len = strlen(yytext);for (int i = 0;i < len;i++) lastNo += yytext[i] == '\n';
						using std::make_pair;
						TreeNode* node = new TreeNode();
						node->lineno = lineNo;
						yylval.type = make_pair(node, Label::NEQ);
						return NEQ;
					}

\&{Ignore}\&				{
lineNo = lastNo;int len = strlen(yytext);for (int i = 0;i < len;i++) lastNo += yytext[i] == '\n';
						using std::make_pair;
						TreeNode* node = new TreeNode();
						node->lineno = lineNo;
						yylval.type = make_pair(node, Label::AND);
						return AND;
					}

\|{Ignore}\|				{
lineNo = lastNo;int len = strlen(yytext);for (int i = 0;i < len;i++) lastNo += yytext[i] == '\n';
						using std::make_pair;
						TreeNode* node = new TreeNode();
						node->lineno = lineNo;
						yylval.type = make_pair(node, Label::OR);
						return OR;
					}

w{Ignore}h{Ignore}i{Ignore}l{Ignore}e				{
lineNo = lastNo;int len = strlen(yytext);for (int i = 0;i < len;i++) lastNo += yytext[i] == '\n';
						using std::make_pair;
						TreeNode* node = new TreeNode();
						node->lineno = lineNo;
						yylval.type = make_pair(node, Label::WHILE);
						return WHILE;	
					}

b{Ignore}r{Ignore}e{Ignore}a{Ignore}k				{
lineNo = lastNo;int len = strlen(yytext);for (int i = 0;i < len;i++) lastNo += yytext[i] == '\n';
						using std::make_pair;
						TreeNode* node = new TreeNode();
						node->lineno = lineNo;
						yylval.type = make_pair(node, Label::BREAK);
						return BREAK;
					}

c{Ignore}o{Ignore}n{Ignore}t{Ignore}i{Ignore}n{Ignore}u{Ignore}e			{
lineNo = lastNo;int len = strlen(yytext);for (int i = 0;i < len;i++) lastNo += yytext[i] == '\n';
						using std::make_pair;
						TreeNode* node = new TreeNode();
						node->lineno = lineNo;
						yylval.type = make_pair(node, Label::CONTINUE);
						return CONTINUE;	
					}

r{Ignore}e{Ignore}t{Ignore}u{Ignore}r{Ignore}n				{
lineNo = lastNo;int len = strlen(yytext);for (int i = 0;i < len;i++) lastNo += yytext[i] == '\n';
						using std::make_pair;
						TreeNode* node = new TreeNode();
						node->lineno = lineNo;
						yylval.type = make_pair(node, Label::RETURN);
						return RETURN;	
					}

i{Ignore}f					{
lineNo = lastNo;int len = strlen(yytext);for (int i = 0;i < len;i++) lastNo += yytext[i] == '\n';
						using std::make_pair;
						TreeNode* node = new TreeNode();
						node->lineno = lineNo;
						yylval.type = make_pair(node, Label::IF);
						return IF;	
					}

e{Ignore}l{Ignore}s{Ignore}e				{
lineNo = lastNo;int len = strlen(yytext);for (int i = 0;i < len;i++) lastNo += yytext[i] == '\n';
						using std::make_pair;
						TreeNode* node = new TreeNode();
						node->lineno = lineNo;
						yylval.type = make_pair(node, Label::ELSE);
						return ELSE;
					}

i{Ignore}n{Ignore}t					{
lineNo = lastNo;int len = strlen(yytext);for (int i = 0;i < len;i++) lastNo += yytext[i] == '\n';
						using std::make_pair;
						TreeNode* node = new TreeNode();
						node->lineno = lineNo;
						yylval.type = make_pair(node, Label::INT);
						return INT;
					}

v{Ignore}o{Ignore}i{Ignore}d 				{
lineNo = lastNo;int len = strlen(yytext);for (int i = 0;i < len;i++) lastNo += yytext[i] == '\n';
						using std::make_pair;
						TreeNode* node = new TreeNode();
						node->lineno = lineNo;
						yylval.type = make_pair(node, Label::VOID);
						return VOID;	
					}

c{Ignore}o{Ignore}n{Ignore}s{Ignore}t 				{
lineNo = lastNo;int len = strlen(yytext);for (int i = 0;i < len;i++) lastNo += yytext[i] == '\n';
						using std::make_pair;
						TreeNode* node = new TreeNode();
						node->lineno = lineNo;
						yylval.type = make_pair(node, Label::CONST);
						return CONST;	
					}

;					{
lineNo = lastNo;int len = strlen(yytext);for (int i = 0;i < len;i++) lastNo += yytext[i] == '\n';
						using std::make_pair;
						TreeNode* node = new TreeNode();
						node->lineno = lineNo;
						yylval.type = make_pair(node, Label::SEMICOLON);
						return SEMICOLON;	
					}

,					{
lineNo = lastNo;int len = strlen(yytext);for (int i = 0;i < len;i++) lastNo += yytext[i] == '\n';
						using std::make_pair;
						TreeNode* node = new TreeNode();
						node->lineno = lineNo;
						yylval.type = make_pair(node, Label::COMMA);
						return COMMA;	
					}

{identifier}		{
lineNo = lastNo;int len = strlen(yytext);for (int i = 0;i < len;i++) lastNo += yytext[i] == '\n';
						using std::make_pair;
						TreeNode* node = new TreeNode();
						node->lineno = lineNo;
						node->token.m_sID = yytext;
						yylval.type = make_pair(node, Label::IDENT);
						return IDENT;
					}

{Const}				{
lineNo = lastNo;int len = strlen(yytext);for (int i = 0;i < len;i++) lastNo += yytext[i] == '\n';
						using std::make_pair;
						TreeNode* node = new TreeNode();
						node->lineno = lineNo;
						int temp;
						if (yytext[0] != '0')	temp = atoi(yytext);
						else if (strlen(yytext) == 1) temp = 0;
						else if (yytext[1] == 'x' || yytext[1] == 'X') sscanf(yytext, "%x",&temp);
						else sscanf(yytext, "%o", &temp);
						node->token.m_nINT = temp;
						yylval.type = make_pair(node, Label::INT_CONST);
						return INT_CONST;
					}

%%


//
int yywrap(void)  
{  
//    fprintf(stderr,"-----the file is end\n");  
    return 1;//返回1表示读取全部结束。如果要接着读其它文件，可以这里fopen该文件，
             //文件指针赋给yyin，并返回0  
}  
/*
void myPretask(){


}
*/
