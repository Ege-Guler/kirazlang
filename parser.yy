%{
#include "lexer.hpp"

#include <kiraz/ast/Operator.h>
#include <kiraz/ast/Literal.h>
#include <kiraz/ast/Keyword.h>

#include <kiraz/token/Literal.h>

int yyerror(const char *msg);
extern std::shared_ptr<Token> curtoken;
extern int yylineno;
%}

%debug

%token REJECTED


%token OP_LPAREN
%token OP_RPAREN

%token OP_PLUS
%token OP_MINUS
%token OP_MULT
%token OP_DIVF

%token KW_LET
%token OP_ASSIGN
%token OP_COLON
%token OP_SEMICOLON

%token IDENTIFIER

%token L_INTEGER L_STRING L_BOOLEAN

%left OP_PLUS OP_MINUS
%left OP_MULT OP_DIVF



%%

stmt:
    OP_LPAREN stmt OP_RPAREN { $$ = $2; }
    | addsub
    | muldiv
    | posneg
    | letstmt
    ;

addsub:
     stmt OP_PLUS stmt { $$ = Node::add<ast::OpAdd>($1, $3); }
    | stmt OP_MINUS stmt { $$ = Node::add<ast::OpSub>($1, $3); }
    ;

muldiv:
     stmt OP_MULT stmt { $$ = Node::add<ast::OpMult>($1, $3); }
    | stmt OP_DIVF stmt { $$ = Node::add<ast::OpDivF>($1, $3); }
    ;

posneg:
    L_INTEGER { $$ = Node::add<ast::Integer>(curtoken); }
    | OP_PLUS stmt { $$ = Node::add<ast::SignedNode>(OP_PLUS, $2); }
    | OP_MINUS stmt { $$ = Node::add<ast::SignedNode>(OP_MINUS, $2); }
    ;

letstmt:
    KW_LET identifier OP_ASSIGN literal OP_SEMICOLON { 
        $$ = Node::add<ast::KwLet>($2, nullptr, $4); 
    }
    | KW_LET identifier OP_COLON identifier OP_SEMICOLON { 
        $$ = Node::add<ast::KwLet>($2, $4, nullptr); 
    }
    | KW_LET identifier OP_COLON identifier OP_ASSIGN stmt OP_SEMICOLON { 
        $$ = Node::add<ast::KwLet>($2, $4, $6); 
    }
    | KW_LET identifier OP_COLON identifier OP_ASSIGN literal OP_SEMICOLON { 
        $$ = Node::add<ast::KwLet>($2, $4, $6); 
    }
    ;

literal:
    L_INTEGER { $$ = Node::add<ast::Integer>(curtoken); }
    ;

identifier:
    IDENTIFIER { $$ = Node::add<ast::Identifier>(curtoken); }
    ;


%%

int yyerror(const char *s) {
    if (curtoken) {
        fmt::print("** Parser Error at {}:{} at token: {}\n",
            yylineno, Token::colno, curtoken->as_string());
    }
    else {
        fmt::print("** Parser Error at {}:{}, null token\n",
            yylineno, Token::colno);
    }
    Token::colno = 0;
    Node::reset_root();
    return 1;
}