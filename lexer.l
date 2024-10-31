
%option noyywrap
%option yylineno

%{
// https://stackoverflow.com/questions/9611682/flexlexer-support-for-unicode/9617585#9617585
#include "main.h"
#include <kiraz/token/Literal.h>
#include <kiraz/token/Operator.h>
#include <kiraz/token/Keyword.h>
static auto &colno = Token::colno;
using namespace token;
extern std::shared_ptr<Token> curtoken;
%}


%%

[0-9]+ { colno += yyleng; curtoken = Token::New<Integer>(10, yytext); return L_INTEGER; }
\"([^"\\]|\\.)*\" { colno += yyleng; curtoken = Token::New<String>(yytext); return L_STRING; }
"+"   { colno += yyleng; curtoken = Token::New<OpPlus>(); return OP_PLUS; }
"-"   { colno += yyleng; curtoken = Token::New<OpMinus>(); return OP_MINUS; }
"*"   { colno += yyleng; curtoken = Token::New<OpMult>(); return OP_MULT; }
"/"   { colno += yyleng; curtoken = Token::New<OpDivF>(); return OP_DIVF; }
"("   {colno += yyleng; curtoken = Token::New<OpLparen>(); return OP_LPAREN; }
")"   { colno += yyleng; curtoken = Token::New<OpRparen>(); return OP_RPAREN; }
"="   { colno += yyleng; curtoken = Token::New<OpAssign>(); return OP_ASSIGN; }
":"   { colno += yyleng; curtoken = Token::New<OpColon>(); return OP_COLON; }
";"   { colno += yyleng; curtoken = Token::New<OpSemiColon>(); return OP_SEMICOLON; }
"{"   { colno += yyleng; curtoken = Token::New<OpLbrace>(); return OP_LBRACE; }
"}"   { colno += yyleng; curtoken = Token::New<OpRbrace>(); return OP_RBRACE; }
","   { colno += yyleng; curtoken = Token::New<OpComma>(); return OP_COMMA; }

"let"    { colno += yyleng; curtoken = Token::New<KwLet>(); return KW_LET; }
"func"   { colno += yyleng; curtoken = Token::New<KwFunc>(); return KW_FUNC; }
"if"     { colno += yyleng; curtoken = Token::New<KwIf>(); return KW_IF; }
"import" { colno += yyleng; curtoken = Token::New<KwImport>(); return KW_IMPORT; }
"else"   { colno += yyleng; curtoken = Token::New<KwElse>(); return KW_ELSE; }
"class"  { colno += yyleng; curtoken = Token::New<KwClass>(); return KW_CLASS; }
"while"  { colno += yyleng; curtoken = Token::New<KwWhile>(); return KW_WHILE; }


[a-zA-Z_][a-zA-Z0-9_]* { colno += yyleng; curtoken = Token::New<Identifier>(yytext); return IDENTIFIER; }

[ \n\t]+ {colno += yyleng;}
.       {colno += yyleng; curtoken = Token::New<Rejected>(yytext); return YYUNDEF; }
