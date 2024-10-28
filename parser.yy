%{
#include "lexer.hpp"

#include <kiraz/ast/Operator.h>
#include <kiraz/ast/Literal.h>
#include <kiraz/ast/Keyword.h>
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

%token KW_LET KW_FUNC KW_IF KW_IMPORT KW_ELSE

%token OP_ASSIGN OP_COLON OP_SEMICOLON OP_COMMA

%token IDENTIFIER

%token L_INTEGER L_STRING L_BOOLEAN

%left OP_PLUS OP_MINUS
%left OP_MULT OP_DIVF

%%

code:
    stmt_or_func_list
    ;

stmt_or_func_list:
    stmt OP_SEMICOLON stmt_or_func_list { $$ = Node::add<ast::NodeList>($1, $3); }
    | stmt OP_SEMICOLON                 { $$ = Node::add<ast::NodeList>($1); }
    | funcstmt stmt_or_func_list         { $$ = Node::add<ast::NodeList>($1, $2); }
    | funcstmt                          { $$ = Node::add<ast::NodeList>($1); }
    ;

stmt:
    expr
    | letstmt
    | assignmentstmt
    | importstmt
    | ifstmt
    | identifier
    ;

assignmentstmt:
    identifier OP_ASSIGN expr { $$ = Node::add<ast::OpAssign>($1, $3); }
    ;
    
importstmt:
    KW_IMPORT identifier { $$ = Node::add<ast::KwImport>($2); }
    ;

ifstmt:
    KW_IF OP_LPAREN identifier OP_RPAREN OP_LBRACE option_then OP_RBRACE option_else { $$ = Node::add<ast::KwIf>($3, $6, $8); }
    ;
    
option_then:
    stmt_or_func_list
    | %empty { $$ = nullptr; }
    ;
    
option_else:
    KW_ELSE ifstmt { $$ = $2; }
    | KW_ELSE OP_LBRACE stmt_or_func_list OP_RBRACE { $$ = $3; }
    | KW_ELSE OP_LBRACE OP_RBRACE { $$ = nullptr; }
    | %empty { $$ = nullptr; }
    ;
    

expr:
    expr OP_PLUS expr { $$ = Node::add<ast::OpAdd>($1, $3); }
    | expr OP_MINUS expr { $$ = Node::add<ast::OpSub>($1, $3); }
    | expr OP_MULT expr { $$ = Node::add<ast::OpMult>($1, $3); }
    | expr OP_DIVF expr { $$ = Node::add<ast::OpDivF>($1, $3); }
    | primary
    ;

primary:
    OP_LPAREN expr OP_RPAREN { $$ = $2; }
    | posneg
    ;

posneg:
    L_INTEGER { $$ = Node::add<ast::Integer>(curtoken); }
    | OP_PLUS primary { $$ = Node::add<ast::SignedNode>(OP_PLUS, $2); }
    | OP_MINUS primary { $$ = Node::add<ast::SignedNode>(OP_MINUS, $2); }
    ;

letstmt:
    KW_LET identifier OP_COLON identifier OP_ASSIGN expr { $$ = Node::add<ast::KwLet>($2, $4, $6); }
    | KW_LET identifier OP_COLON identifier { $$ = Node::add<ast::KwLet>($2, $4, nullptr); }
    | KW_LET identifier OP_ASSIGN expr { $$ = Node::add<ast::KwLet>($2, nullptr, $4); }
    ;

funcstmt:
    KW_FUNC identifier OP_LPAREN arglist OP_RPAREN OP_COLON identifier OP_LBRACE stmt_or_func_list OP_RBRACE {
        $$ = Node::add<ast::KwFunc>($2, $4, $7, $9);
    }
    ;

arglist:
    identifier OP_COLON identifier OP_COMMA arglist { $$ = Node::add<ast::ArgList>($1, $3, $5); }
    | identifier OP_COLON identifier { $$ = Node::add<ast::ArgList>($1, $3, nullptr); }
    | %empty { $$ = nullptr; }
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
    } else {
        fmt::print("** Parser Error at {}:{}, null token\n",
            yylineno, Token::colno);
    }
    Token::colno = 0;
    Node::reset_root();
    return 1;
}
