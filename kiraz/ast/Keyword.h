#ifndef KIRAZ_AST_KEYWORD_H
#define KIRAZ_AST_KEYWORD_H

#include <cassert>
#include <kiraz/Node.h>
#include <vector>

namespace ast {

class KwLet : public Node {
public:
    explicit KwLet(const Node::Ptr &identifier, const Node::Ptr &type, const Node::Ptr &literal)
            : Node(KW_LET), m_identifier(identifier), m_type(type), m_initial_val(literal) {
        assert(identifier);
    }

    std::string as_string() const override {
        if (m_type == nullptr) {
            return fmt::format(
                    "Let(n={}, i={})", m_identifier->as_string(), m_initial_val->as_string());
        }
        else if (m_initial_val == nullptr) {
            return fmt::format("Let(n={}, t={})", m_identifier->as_string(), m_type->as_string());
        }
        return fmt::format("Let(n={}, t={}, i={})", m_identifier->as_string(), m_type->as_string(),
                m_initial_val->as_string());
    }

private:
    Node::Ptr m_identifier;
    Node::Ptr m_type;
    Node::Ptr m_initial_val;
};

class KwFunc : public Node {
public:
    explicit KwFunc(const Node::Ptr &name, const Node::Ptr &args, const Node::Ptr &rtype,
            const Node::Ptr &scope)
            : Node(KW_FUNC), m_name(name), m_args(args), m_rtype(rtype), m_scope(scope) {
        assert(name);
        assert(rtype);
    }

    std::string as_string() const override {
        std::string result = fmt::format("Func(n={}", m_name->as_string());

        if (m_args) {
            
            std::string args_str = m_args->as_string();
            
            size_t pos = 0;
            while ((pos = args_str.find("], [", pos)) != std::string::npos) {
                args_str.replace(pos, 4, ", ");
            }
            
            if (args_str.find("FuncArgs([") == 0 && args_str.rfind("])") == args_str.size() - 2) {
                args_str = args_str.substr(10, args_str.size() - 12);
            }
            
            result += fmt::format(", a=FuncArgs([{}])", args_str);
            
        } else {
            result += ", a=[]";
        }

        result += fmt::format(", r={}", m_rtype->as_string());

        if (m_scope) {
            result += fmt::format(", s=[{}])", m_scope->as_string());
        } else {
            result += ", s=[])";
        }

        return result;
    }

private:
    Node::Ptr m_name;
    Node::Ptr m_args;
    Node::Ptr m_rtype;
    Node::Ptr m_scope;
};


class KwImport : public Node {
public:
    explicit KwImport(const Node::Ptr &identifier) : Node(KW_IMPORT), m_identifier(identifier) {
        assert(identifier);
    }

    std::string as_string() const override {
        return fmt::format("Import({})", m_identifier->as_string());
    }

private:
    Node::Ptr m_identifier;
};

class KwIf : public Node {
public:
    explicit KwIf(
            const Node::Ptr &condition, const Node::Ptr &then_block, const Node::Ptr &else_block)
            : Node(KW_IF)
            , m_condition(condition)
            , m_then_block(then_block)
            , m_else_block(else_block) {
        assert(condition);
    }

    std::string as_string() const override {
        return fmt::format("If(?={}, then=[{}], else=[{}])", m_condition->as_string(),
                m_then_block ? m_then_block->as_string() : "",
                m_else_block ? m_else_block->as_string() : "");
    }

private:
    Node::Ptr m_condition;
    Node::Ptr m_then_block;
    Node::Ptr m_else_block;
};

class KwClass : public Node {
public:
    KwClass(const Node::Ptr &name, const Node::Ptr &stmt_list)
            : Node(KW_CLASS), m_name(name), m_stmt_list(stmt_list ? stmt_list : nullptr) {
        assert(name);
    }

    std::string as_string() const override {
        if (m_stmt_list) {
            return fmt::format("Class(n={}, s=[{}])", m_name->as_string(),
                    m_stmt_list->as_string());
        }
        else {
            return fmt::format("Class(n={}, s=[])", m_name->as_string());
        }
    }

private:
    Node::Ptr m_name;
    Node::Ptr m_stmt_list;
};

class KwWhile : public Node {
public:
    KwWhile(const Node::Ptr &condition, const Node::Ptr &stmt_list)
            : Node(KW_WHILE), m_condition(condition), m_stmt_list(stmt_list ? stmt_list : nullptr) {
        assert(condition);
    }
    std::string as_string() const override {
        return fmt::format("While(?={}, repeat=[{}])", 
                            m_condition->as_string(),
                            m_stmt_list ? m_stmt_list->as_string(): "");
    }

private:
    Node::Ptr m_condition;
    Node::Ptr m_stmt_list;
};

class KwReturn : public Node {
public:
    explicit KwReturn(const Node::Ptr &value)
        : Node(KW_RETURN), m_value(value) {
        assert(value);
    }

    std::string as_string() const override {
        return fmt::format("Return({})", m_value->as_string());
    }

private:
    Node::Ptr m_value;
};


} // namespace ast

#endif // KIRAZ_AST_KEYWORD_H
