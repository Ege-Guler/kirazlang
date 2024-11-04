%{
#include "lexer.hpp"

#include <kiraz/ast/Operator.h>
#include <kiraz/ast/Literal.h>
#include <kiraz/ast/Keyword.h>
#include <kiraz/ast/Misc.h>
#include <kiraz/token/Literal.h>
#include <vector>

int yyerror(const char *msg);
extern std::shared_ptr<Token> curtoken;
extern int yylineno;
%}

%debug

%token REJECTED

%token OP_LPAREN OP_RPAREN
%token OP_LBRACE OP_RBRACE

%token OP_PLUS OP_MINUS OP_MULT OP_DIVF

%token KW_LET KW_FUNC KW_IF KW_IMPORT KW_ELSE KW_CLASS KW_WHILE

%token OP_ASSIGN OP_COLON OP_SEMICOLON OP_COMMA OP_DOT

%token OP_EQ OP_GT OP_GE OP_LT OP_LE

%token KW_RETURN

%token IDENTIFIER

%token L_INTEGER L_STRING L_BOOLEAN

%left OP_PLUS OP_MINUS
%left OP_MULT OP_DIVF

%nonassoc OP_EQ OP_GT OP_GE OP_LT OP_LE
%precedence OP_LPAREN

%%

code:
    stmt_list { $$ = Node::add<ast::Module>($1); }
    ;


stmt_list:
    stmt OP_SEMICOLON stmt_list { $$ = Node::add<ast::NodeList>($1, $3); }
    | stmt OP_SEMICOLON         { $$ = Node::add<ast::NodeList>($1); }
    ;

stmt:
    expr
    | classstmt
    | letstmt
    | assignmentstmt
    | importstmt
    | ifstmt
    | identifier
    | funcstmt
    | whilestmt
    | literal
    | returnstmt
    ;

returnstmt:
KW_RETURN expr {
    if(!$2) {
        $$ = nullptr;
    }else {$$ = Node::add<ast::KwReturn>($2); }}
    ;
    
whilestmt:
    KW_WHILE OP_LPAREN expr OP_RPAREN OP_LBRACE stmt_list OP_RBRACE
    { $$ = Node::add<ast::KwWhile>($3, $6); }
    |     KW_WHILE OP_LPAREN expr OP_RPAREN OP_LBRACE OP_RBRACE
    { $$ = Node::add<ast::KwWhile>($3, nullptr); }
    ;

classstmt:
    KW_CLASS identifier OP_LBRACE class_body OP_RBRACE
    { $$ = Node::add<ast::KwClass>($2, $4); }
    ;

class_body:
    class_member_list { $$ = $1; }
    | %empty { $$ = nullptr; }
    ;

class_member_list:
    class_member OP_SEMICOLON class_member_list { $$ = Node::add<ast::NodeList>($1, $3); }
    | class_member OP_SEMICOLON { $$ = Node::add<ast::NodeList>($1); }
    ;

class_member:
    letstmt
    | funcstmt
    | %empty { $$ = nullptr; }
    ;
    
assignmentstmt:
    identifier OP_ASSIGN expr
    {
        if (!$3) {
            $$ = nullptr;
        } else {
            $$ = Node::add<ast::OpAssign>($1, $3);
        }
    }
    ;
    
    
importstmt:
    KW_IMPORT identifier { $$ = Node::add<ast::KwImport>($2); }
    ;

ifstmt:
    KW_IF OP_LPAREN expr OP_RPAREN OP_LBRACE option_then OP_RBRACE option_else { $$ = Node::add<ast::KwIf>($3, $6, $8); }
    ;
    
option_then:
    stmt_list
    | %empty { $$ = nullptr; }
    ;
    
option_else:
    KW_ELSE ifstmt { $$ = $2; }
    | KW_ELSE OP_LBRACE stmt_list OP_RBRACE { $$ = $3; }
    | KW_ELSE OP_LBRACE OP_RBRACE { $$ = nullptr; }
    | %empty { $$ = nullptr; }
    ;
    

expr:
    exprparen
    | expr OP_PLUS expr { $$ = Node::add<ast::OpAdd>($1, $3); }
    | expr OP_MINUS expr { $$ = Node::add<ast::OpSub>($1, $3); }
    | expr OP_MULT expr { $$ = Node::add<ast::OpMult>($1, $3); }
    | expr OP_DIVF expr { $$ = Node::add<ast::OpDivF>($1, $3); }
    | expr OP_EQ expr { $$ = Node::add<ast::OpEq>($1, $3); }
    | expr OP_GT expr { $$ = Node::add<ast::OpGt>($1, $3); }
    | expr OP_GE expr { $$ = Node::add<ast::OpGe>($1, $3); }
    | expr OP_LT expr { $$ = Node::add<ast::OpLt>($1, $3); }
    | expr OP_LE expr { $$ = Node::add<ast::OpLe>($1, $3); }
    | expr OP_DOT identifier { $$ = Node::add<ast::OpDot>($1, $3); }
    | expr OP_LPAREN calllist OP_RPAREN { $$ = Node::add<ast::Call>($1, $3); }
    ;
    
exprparen:
    OP_LPAREN expr OP_RPAREN { $$ = $2; }
    | posneg
    | literal
    | identifier
    | bool
    ;

posneg:
    L_INTEGER { $$ = Node::add<ast::Integer>(curtoken); }
    | OP_PLUS exprparen { $$ = Node::add<ast::SignedNode>(OP_PLUS, $2); }
    | OP_MINUS exprparen { $$ = Node::add<ast::SignedNode>(OP_MINUS, $2); }
    ;

letstmt:
    KW_LET identifier OP_COLON identifier OP_ASSIGN expr { $$ = Node::add<ast::KwLet>($2, $4, $6); }
    | KW_LET identifier OP_COLON identifier { $$ = Node::add<ast::KwLet>($2, $4, nullptr); }
    | KW_LET identifier OP_ASSIGN expr { $$ = Node::add<ast::KwLet>($2, nullptr, $4); }
    ;

funcstmt:
    KW_FUNC identifier OP_LPAREN arglist OP_RPAREN OP_COLON identifier OP_LBRACE stmt_list OP_RBRACE {
        $$ = Node::add<ast::KwFunc>($2, $4, $7, $9);
    }
    |     KW_FUNC identifier OP_LPAREN arglist OP_RPAREN OP_COLON identifier OP_LBRACE OP_RBRACE {
        $$ = Node::add<ast::KwFunc>($2, $4, $7, nullptr);
    }
    ;
    
arglist:
    identifier OP_COLON identifier OP_COMMA arglist { $$ = Node::add<ast::ArgList>($1, $3, $5); }
    | identifier OP_COLON identifier { $$ = Node::add<ast::ArgList>($1, $3, nullptr); }
    | %empty { $$ = nullptr; }
    ;

calllist:
    expr OP_COMMA calllist { $$ = Node::add<ast::CallList>($1, $3); }
    | expr { $$ = Node::add<ast::CallList>($1); }
    | %empty { $$ = nullptr; }
    ;


literal:
    | L_STRING  { $$ = Node::add<ast::String>(curtoken); }
    ;

bool :
    L_BOOLEAN { $$ = Node::add<ast::Bool>(curtoken); }
    ;
identifier:
    IDENTIFIER { $$ = Node::add<ast::Identifier>(curtoken); }
    ;

%%

int yyerror(const char *s) {
    if (curtoken) {
        fmt::print("** Parser Error at {}:{} at token: {}\n",
            yylineno, Token::colno, curtoken->as_string());
    } else {
        fmt::print("** Parser Error at {}:{}, null token\n",
            yylineno, Token::colno);
    }
    Token::colno = 0;
    Node::reset_root();
    return 1;
}
