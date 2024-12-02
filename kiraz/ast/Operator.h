#ifndef KIRAZ_AST_OPERATOR_H
#define KIRAZ_AST_OPERATOR_H

#include <cassert>

#include <iostream>
#include <kiraz/Compiler.h>
#include <kiraz/Node.h>
#include <kiraz/ast/Keyword.h>

namespace ast {
class OpBinary : public Node {
protected:
    explicit OpBinary(int op, const Node::Ptr &left, const Node::Ptr &right)
            : Node(op), m_left(left), m_right(right) {
        assert(left);
        assert(right);
    }

public:
    auto get_left() const { return m_left; }
    auto get_right() const { return m_right; }

    std::string as_string() const override {
        assert(get_left());
        assert(get_right());

        std::string opstr;
        switch (get_id()) {
        case OP_PLUS:
            opstr = "Add";
            break;
        case OP_MINUS:
            opstr = "Sub";
            break;
        case OP_MULT:
            opstr = "Mult";
            break;
        case OP_DIVF:
            opstr = "DivF";
            break;
        case OP_ASSIGN:
            opstr = "Assign";
            break;
        case OP_EQ:
            opstr = "OpEq";
            break;
        case OP_GT:
            opstr = "OpGt";
            break;
        case OP_GE:
            opstr = "OpGe";
            break;
        case OP_LT:
            opstr = "OpLt";
            break;
        case OP_LE:
            opstr = "OpLe";
            break;
        case OP_DOT:
            opstr = "Dot";
            break;
        default:
            break;
        }

        return fmt::format(
                "{}(l={}, r={})", opstr, get_left()->as_string(), get_right()->as_string());
    }

private:
    Node::Ptr m_left, m_right;
};

class OpAdd : public OpBinary {
public:
    OpAdd(const Node::Ptr &left, const Node::Ptr &right) : OpBinary(OP_PLUS, left, right) {}

    Ptr compute_stmt_type(SymbolTable &st) override {
        set_cur_symtab(st.get_cur_symtab());

        auto r = get_right();
        auto l = get_left();

        if (r) {
            if (auto ret = r->compute_stmt_type(st)) {
                return ret;
            }
        }
        if (l) {
            if (auto ret = l->compute_stmt_type(st)) {
                return ret;
            }
        }

        auto l_iden = std::dynamic_pointer_cast<ast::Identifier>(l);
        auto l_node = st.get_symbol(l_iden->get_value());

        auto r_iden = std::dynamic_pointer_cast<ast::Identifier>(r);
        auto r_node = st.get_symbol(r_iden->get_value());

        if (l_node) {
            if ((r_node.stmt)->get_type(st) != (l_node.stmt)->get_type(st)) {
                return set_error(fmt::format("Operator '+' not defined for types '{}' and '{}'",
                        (l_node.stmt)->get_type(st), (r_node.stmt)->get_type(st)));
            }
        }

        return nullptr;
    }
};

class OpSub : public OpBinary {
public:
    OpSub(const Node::Ptr &left, const Node::Ptr &right) : OpBinary(OP_MINUS, left, right) {}
};

class OpMult : public OpBinary {
public:
    OpMult(const Node::Ptr &left, const Node::Ptr &right) : OpBinary(OP_MULT, left, right) {}
};

class OpDivF : public OpBinary {
public:
    OpDivF(const Node::Ptr &left, const Node::Ptr &right) : OpBinary(OP_DIVF, left, right) {}
};

class OpAssign : public OpBinary {
public:
    OpAssign(const Node::Ptr &left, const Node::Ptr &right) : OpBinary(OP_ASSIGN, left, right) {}

    Ptr compute_stmt_type(SymbolTable &st) override {
        set_cur_symtab(st.get_cur_symtab());

        auto r = get_right();
        auto l = get_left();

        if (r) {
            if (auto ret = r->compute_stmt_type(st)) {
                return ret;
            }
        }
        if (l) {
            if (auto ret = l->compute_stmt_type(st)) {
                return ret;
            }
        }

        auto l_iden = std::dynamic_pointer_cast<ast::Identifier>(l);
        auto l_node = st.get_symbol(l_iden->get_value());

        if (l_node) {
            if (r->get_type(st) != (l_node.stmt)->get_type(st)) {
                return set_error(fmt::format(
                        "Left type '{}' of assignment does not match the right type '{}'",
                        (l_node.stmt)->get_type(st), r->get_type(st)));
            }
        }

        return nullptr;
    }
};

class OpEq : public OpBinary {
public:
    OpEq(const Node::Ptr &left, const Node::Ptr &right) : OpBinary(OP_EQ, left, right) {}

    std::string get_type(SymbolTable &st) const override { return "Boolean"; }
};

class OpGt : public OpBinary {
public:
    OpGt(const Node::Ptr &left, const Node::Ptr &right) : OpBinary(OP_GT, left, right) {}

    std::string get_type(SymbolTable &st) const override { return "Boolean"; }
};

class OpGe : public OpBinary {
public:
    OpGe(const Node::Ptr &left, const Node::Ptr &right) : OpBinary(OP_GE, left, right) {}

    std::string get_type(SymbolTable &st) const override { return "Boolean"; }
};

class OpLt : public OpBinary {
public:
    OpLt(const Node::Ptr &left, const Node::Ptr &right) : OpBinary(OP_LT, left, right) {}

    std::string get_type(SymbolTable &st) const override { return "Boolean"; }
};

class OpLe : public OpBinary {
public:
    OpLe(const Node::Ptr &left, const Node::Ptr &right) : OpBinary(OP_LE, left, right) {}

    std::string get_type(SymbolTable &st) const override { return "Boolean"; }
};

class OpDot : public OpBinary {
public:
    OpDot(const Node::Ptr &left, const Node::Ptr &right) : OpBinary(OP_DOT, left, right) {}

    Ptr compute_stmt_type(SymbolTable &st) override {
        set_cur_symtab(st.get_cur_symtab());

        auto l = get_left();
        auto r = get_right();

        if (l) {
            if (auto ret = l->compute_stmt_type(st)) {
                return ret;
            }
        }

        auto r_iden = std::dynamic_pointer_cast<ast::Identifier>(r);
        auto l_iden = std::dynamic_pointer_cast<ast::Identifier>(l);

        if (std::islower(l_iden->get_value()[0])) {
            auto let_node = st.get_symbol(l_iden->get_value());
            auto type = let_node.stmt->get_type(st); // string type name
            auto class_node = st.get_symbol(type);
            if ((class_node.stmt)->get_subsymbol(r)) {
                return nullptr;
            }
            else {
                return set_error(fmt::format("Identifier '{}.{}' is not found", l_iden->get_value(),
                        r_iden->get_value()));
            }
        }
        return nullptr;
    }
};

class OpAnd : public OpBinary {
public:
    OpAnd(const Node::Ptr &left, const Node::Ptr &right) : OpBinary(OP_AND, left, right) {}

    std::string get_type(SymbolTable &st) const override { return "Boolean"; }

    std::string as_string() const override {
        return fmt::format("And(l={}, r={})", get_left()->as_string(), get_right()->as_string());
    }
};

class OpOr : public OpBinary {
public:
    OpOr(const Node::Ptr &left, const Node::Ptr &right) : OpBinary(OP_OR, left, right) {}

    std::string get_type(SymbolTable &st) const override { return "Boolean"; }

    std::string as_string() const override {
        return fmt::format("Or(l={}, r={})", get_left()->as_string(), get_right()->as_string());
    }
};

class OpNot : public Node {
public:
    explicit OpNot(const Node::Ptr &operand) : Node(OP_NOT), m_operand(operand) { assert(operand); }

    std::string get_type(SymbolTable &st) const override { return "Boolean"; }

    std::string as_string() const override {
        return fmt::format("Not(operand={})", m_operand->as_string());
    }

    Node::Ptr get_operand() const { return m_operand; }

private:
    Node::Ptr m_operand;
};

}

#endif // KIRAZ_AST_OPERATOR_H
