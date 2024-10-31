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
            result += fmt::format(", a={}", m_args->as_string());
        }
        else {
            result += ", a=[]";
        }
        result += fmt::format(", r={}", m_rtype->as_string());

        if(m_scope){
            result += fmt::format(", s=[{}])", m_scope->as_string());

        }else{
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
    explicit KwIf(const Node::Ptr &condition, const Node::Ptr &then_block, const Node::Ptr &else_block)
        : Node(KW_IF), m_condition(condition), m_then_block(then_block), m_else_block(else_block) {
        assert(condition);
    }

    std::string as_string() const override {
        return fmt::format("If(?={}, then=[{}], else=[{}])",
                           m_condition->as_string(),
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
                // Wrap statements in CStmtList if there are any statements in the list
                return fmt::format("Class(n={}, s=CStmtList([{}]))",
                                   m_name->as_string(),
                                   m_stmt_list->as_string());
            } else {
                // Empty class body
                return fmt::format("Class(n={}, s=[])",
                                   m_name->as_string());
            }
        }

private:
    Node::Ptr m_name;
    Node::Ptr m_stmt_list;
};


class NodeList : public Node {
public:
    explicit NodeList(const Node::Ptr &node) : Node(1) { m_nodes.push_back(node); }

    NodeList(const Node::Ptr &first, const Node::Ptr &rest) : Node(1) {
        m_nodes.push_back(first);
        auto rest_list = std::dynamic_pointer_cast<NodeList>(rest);
        if (rest_list) {
            m_nodes.insert(m_nodes.end(), rest_list->m_nodes.begin(), rest_list->m_nodes.end());
        }
    }

    std::string as_string() const override {
        std::string result;
        for (const auto &node : m_nodes) {
            result += node->as_string() + ", ";
        }
        result.erase(result.length() - 2, 2);
        return result;
    }

private:
    std::vector<Node::Ptr> m_nodes;
};
class ArgList : public Node {
public:
    explicit ArgList(const Node::Ptr &identifier, const Node::Ptr &type) : Node(0) {
        m_args.emplace_back(identifier, type);
    }

    ArgList(const Node::Ptr &first, const Node::Ptr &first_type, const Node::Ptr &rest) : Node(0) {
        m_args.emplace_back(first, first_type);
        auto rest_list = std::dynamic_pointer_cast<ArgList>(rest);
        if (rest_list) {
            m_args.insert(m_args.end(), rest_list->m_args.begin(), rest_list->m_args.end());
        }
    }

    std::string as_string() const override {
        std::string result = "FuncArgs(";
        for (const auto &arg : m_args) {
            result += "[FArg(n=";
            result +=
                    std::get<0>(arg)->as_string() + ", t=" + std::get<1>(arg)->as_string() + ")], ";
        }

        result.erase(result.length() - 2, 2);
        result += ")";
        return result;
    }

private:
    std::vector<std::pair<Node::Ptr, Node::Ptr>> m_args;
};

} // namespace ast

#endif // KIRAZ_AST_KEYWORD_H
