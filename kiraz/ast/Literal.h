#ifndef KIRAZ_AST_LITERAL_H
#define KIRAZ_AST_LITERAL_H

#include <iostream>
#include <kiraz/Compiler.h>
#include <kiraz/Node.h>

namespace ast {

class Bool : public Node {
public:
    Bool() : Node(L_BOOLEAN) {}
    Bool(Token::Ptr);
    std::string as_string() const override {
        return fmt::format("Bool({})", m_value ? "true" : "false");
    }

    std::string get_type() const override { return "Boolean"; }

private:
    int64_t m_value;
};

class Integer : public Node {
public:
    Integer(Token::Ptr);

    std::string as_string() const override { return fmt::format("Int({})", m_value); }

    std::string get_type() const override { return "Integer64"; }

private:
    int64_t m_value;
    int64_t m_base;
};

class String : public Node {
public:
    String(Token::Ptr);

    std::string as_string() const override { return fmt::format("Str({})", m_value); }

    std::string get_type() const override { return "String"; }

private:
    std::string m_value;
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
    Identifier(std::string value);

    virtual ~Identifier() {}

    bool is_identifier() const override { return true; }

    std::string as_string() const override { return fmt::format("Id({})", m_value); }

    std::string get_value() const { return m_value; }

    std::string get_type() const override { return "Identifier"; }

    Node::Ptr add_to_symtab_forward(SymbolTable &st) override {
        st.add_symbol(m_value, shared_from_this());
        return nullptr;
    }

    Node::Ptr add_to_symtab_ordered(SymbolTable &st) override {
        st.add_symbol(m_value, shared_from_this());
        return nullptr;
    }

    Node::Ptr compute_stmt_type(SymbolTable &st) override {

        if (! st.get_symbol(m_value)) {

            return set_error(fmt::format("Identifier '{}' is not found", m_value));
        }

        return nullptr;
    }

private:
    std::string m_value;
};

}

#endif
