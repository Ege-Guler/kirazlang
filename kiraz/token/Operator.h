#ifndef KIRAZ_TOKEN_OPERATOR_H
#define KIRAZ_TOKEN_OPERATOR_H

#include <kiraz/Token.h>

namespace token {

class Operator : public Token {
public:
    Operator(int type) : Token(type) {}
};

class OpPlus : public Operator {
public:
    OpPlus() : Operator(OP_PLUS) {}
    std::string as_string() const override { return "OP_PLUS"; }
};

class OpMinus : public Operator {
public:
    OpMinus() : Operator(OP_MINUS) {}
    std::string as_string() const override { return "OP_MINUS"; }
};

class OpMult : public Operator {
public:
    OpMult() : Operator(OP_MULT) {}
    std::string as_string() const override { return "OP_MULT"; }
};

class OpDivF : public Operator {
public:
    OpDivF() : Operator(OP_DIVF) {}
    std::string as_string() const override { return "OP_DIVF"; }
};

class OpLparen : public Operator {
public:
    OpLparen() : Operator(OP_LPAREN) {}
    std::string as_string() const override { return "OP_LPAREN"; }
};

class OpRparen : public Operator {
public:
    OpRparen() : Operator(OP_RPAREN) {}
    std::string as_string() const override { return "OP_RPAREN"; }
};

class OpLbrace : public Operator {
public:
    OpLbrace() : Operator(OP_LBRACE) {}
    std::string as_string() const override { return "OP_LBRACE"; }
};

class OpRbrace : public Operator {
public:
    OpRbrace() : Operator(OP_RBRACE) {}
    std::string as_string() const override { return "OP_RBRACE"; }
};
class OpAssign : public Operator {
public:
    OpAssign() : Operator(OP_ASSIGN) {}
    std::string as_string() const override { return "OP_ASSIGN"; }
};

class OpColon : public Operator {
public:
    OpColon() : Operator(OP_COLON) {}
    std::string as_string() const override { return "OP_COLON"; }
};

class OpSemiColon : public Operator {
public:
    OpSemiColon() : Operator(OP_SEMICOLON) {}
    std::string as_string() const override { return "OP_SEMICOLON"; }
};

class OpComma : public Operator {
public:
    OpComma() : Operator(OP_COMMA) {}
    std::string as_string() const override { return "OP_COMMA"; }
};

class OpEq : public Operator {
public:
    OpEq() : Operator(OP_EQ) {}
    std::string as_string() const override { return "OP_EQ"; }
};

class OpGt : public Operator {
public:
    OpGt() : Operator(OP_GT) {}
    std::string as_string() const override { return "OP_GT"; }
};

class OpGe : public Operator {
public:
    OpGe() : Operator(OP_GE) {}
    std::string as_string() const override { return "OP_GE"; }
};

class OpLt : public Operator {
public:
    OpLt() : Operator(OP_LT) {}
    std::string as_string() const override { return "OP_LT"; }
};

class OpLe : public Operator {
public:
    OpLe() : Operator(OP_LE) {}
    std::string as_string() const override { return "OP_LE"; }
};

class OpDot : public Operator {
public:
    OpDot() : Operator(OP_DOT) {}
    std::string as_string() const override { return "DOT"; }
};

class OpAnd : public Operator {
public:
    OpAnd() : Operator(OP_AND) {}
    std::string as_string() const override { return "OP_AND"; }
};

class OpOr : public Operator {
public:
    OpOr() : Operator(OP_OR) {}
    std::string as_string() const override { return "OP_OR"; }
};

class OpNot : public Operator {
public:
    OpNot() : Operator(OP_NOT) {}
    std::string as_string() const override { return "OP_NOT"; }
};


}

#endif // KIRAZ_TOKEN_OPERATOR_H
