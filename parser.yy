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
%precedence UNARY

%%

code:
    stmt_list { $$ = Node::add<ast::Module>($1); }
    ;


stmt_list:
    stmt OP_SEMICOLON stmt_list { $$ = Node::add<ast::NodeList>($1, $3); }
    | stmt OP_SEMICOLON         { $$ = Node::add<ast::NodeList>($1); }
    ;

class_list:
    stmt_list
    | %empty { $$ = nullptr; }
    ;

stmt:
    expr
    | classstmt
    | letstmt
    | assignmentstmt
    | importstmt
    | ifstmt
    | funcstmt
    | whilestmt
    | returnstmt
    ;

classstmt:
    KW_CLASS identifier OP_LBRACE class_list OP_RBRACE
    { $$ = Node::add<ast::KwClass>($2, $4); }
    ;

letstmt:
    KW_LET identifier OP_COLON identifier OP_ASSIGN expr { $$ = Node::add<ast::KwLet>($2, $4, $6); }
    | KW_LET identifier OP_COLON identifier { $$ = Node::add<ast::KwLet>($2, $4, nullptr); }
    | KW_LET identifier OP_ASSIGN expr { $$ = Node::add<ast::KwLet>($2, nullptr, $4); }
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
    
whilestmt:
    KW_WHILE OP_LPAREN expr OP_RPAREN OP_LBRACE stmt_list OP_RBRACE
    { $$ = Node::add<ast::KwWhile>($3, $6); }
    |     KW_WHILE OP_LPAREN expr OP_RPAREN OP_LBRACE OP_RBRACE
    { $$ = Node::add<ast::KwWhile>($3, nullptr); }
    ;

funcstmt:
    KW_FUNC identifier OP_LPAREN arglist OP_RPAREN OP_COLON identifier OP_LBRACE stmt_list OP_RBRACE {
        $$ = Node::add<ast::KwFunc>($2, $4, $7, $9);
    }
    |     KW_FUNC identifier OP_LPAREN arglist OP_RPAREN OP_COLON identifier OP_LBRACE OP_RBRACE {
        $$ = Node::add<ast::KwFunc>($2, $4, $7, nullptr);
    }
    ;
    
returnstmt:
    KW_RETURN expr {
        if(!$2) {
            $$ = nullptr;
        }else {$$ = Node::add<ast::KwReturn>($2); }
    }
    ;

expr:
    comp_expr
    ;

comp_expr:
    addsub_expr
    | comp_expr OP_EQ addsub_expr { $$ = Node::add<ast::OpEq>($1, $3); }
    | comp_expr OP_GT addsub_expr { $$ = Node::add<ast::OpGt>($1, $3); }
    | comp_expr OP_GE addsub_expr { $$ = Node::add<ast::OpGe>($1, $3); }
    | comp_expr OP_LT addsub_expr { $$ = Node::add<ast::OpLt>($1, $3); }
    | comp_expr OP_LE addsub_expr { $$ = Node::add<ast::OpLe>($1, $3); }
    ;

addsub_expr:
    muldiv_expr
    | addsub_expr OP_PLUS muldiv_expr { $$ = Node::add<ast::OpAdd>($1, $3); }
    | addsub_expr OP_MINUS muldiv_expr { $$ = Node::add<ast::OpSub>($1, $3); }
    ;

muldiv_expr:
    dot_expr
    | muldiv_expr OP_MULT dot_expr { $$ = Node::add<ast::OpMult>($1, $3); }
    | muldiv_expr OP_DIVF dot_expr { $$ = Node::add<ast::OpDivF>($1, $3); }
    ;

dot_expr:
    primary_expr
    | dot_expr OP_DOT identifier { $$ = Node::add<ast::OpDot>($1, $3); }
    | dot_expr OP_LPAREN calllist OP_RPAREN %prec OP_LPAREN { $$ = Node::add<ast::Call>($1, $3); }
    ;

primary_expr:
    OP_LPAREN expr OP_RPAREN { $$ = $2; }
    | unary_expr
    | literal
    | identifier
    | bool
    ;

unary_expr:
    OP_PLUS primary_expr %prec UNARY { $$ = Node::add<ast::SignedNode>(OP_PLUS, $2); }
    | OP_MINUS primary_expr %prec UNARY { $$ = Node::add<ast::SignedNode>(OP_MINUS, $2); }
    | L_INTEGER { $$ = Node::add<ast::Integer>(curtoken); }
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
    L_STRING  { $$ = Node::add<ast::String>(curtoken); }
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
