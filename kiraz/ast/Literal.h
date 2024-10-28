#ifndef KIRAZ_AST_LITERAL_H
#define KIRAZ_AST_LITERAL_H

#include <kiraz/Node.h>

namespace ast {
class Integer : public Node {
public:
    Integer(Token::Ptr);

    std::string as_string() const override { return fmt::format("Int({}, {})", m_base, m_value); }

private:
    int64_t m_value;
    int64_t m_base;
};

class SignedNode : public Node {
public:
    SignedNode(int op, Node::Cptr operand) : Node(op), m_operator(op), m_operand(operand) {}

    std::string as_string() const override {

        return fmt::format("Signed({}, {})", m_operator == OP_MINUS ? "OP_MINUS" : "OP_PLUS",
                m_operand->as_string());
    }

private:
    int m_operator;
    Node::Cptr m_operand;
};

class Identifier : public Node {
public:
    Identifier(Token::Ptr);

    std::string as_string() const override { return fmt::format("Id({})", m_value); }

private:
    std::string m_value;
};

}

#endif
