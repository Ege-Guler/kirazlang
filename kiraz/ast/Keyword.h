#ifndef KIRAZ_AST_KEYWORD_H
#define KIRAZ_AST_KEYWORD_H

#include <cassert>
#include <vector>
#include <kiraz/Node.h>

namespace ast {

class KwLet : public Node {
public:
    explicit KwLet(const Node::Ptr &identifier, const Node::Ptr &type, const Node::Ptr &literal)
        : Node(KW_LET), m_identifier(identifier), m_type(type), m_initial_val(literal) {
        assert(identifier);
    }

    std::string as_string() const override {
        if (m_type == nullptr) {
            return fmt::format("Let(n={}, i={})", m_identifier->as_string(), m_initial_val->as_string());
        } else if (m_initial_val == nullptr) {
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
    explicit KwFunc(const Node::Ptr &name, const Node::Ptr &args, const Node::Ptr &rtype, const Node::Ptr &scope)
        : Node(KW_FUNC), m_name(name), m_args(args), m_rtype(rtype), m_scope(scope) {
        assert(name);
        assert(rtype);
        assert(scope);
    }

    std::string as_string() const override {
        std::string result = fmt::format("Func(n={})", m_name->as_string());
        if (m_args) {
            result += fmt::format(", args=[{}]", m_args->as_string());
        } else {
            result += ", args=[]";
        }
        result += fmt::format(", rtype={}", m_rtype->as_string());
        result += fmt::format(", scope={}", m_scope->as_string());

        return result;
    }

private:
    Node::Ptr m_name;   
    Node::Ptr m_args;   
    Node::Ptr m_rtype;  
    Node::Ptr m_scope;
};

class NodeList : public Node {
public:
    explicit NodeList(const Node::Ptr &node) : Node(1) {
        m_nodes.push_back(node);
    }

    NodeList(const Node::Ptr &first, const Node::Ptr &rest) : Node(1) {
        m_nodes.push_back(first);
        auto rest_list = std::dynamic_pointer_cast<NodeList>(rest);
        if (rest_list) {
            m_nodes.insert(m_nodes.end(), rest_list->m_nodes.begin(), rest_list->m_nodes.end());
        }
    }

    std::string as_string() const override {
        std::string result = "NodeList(";
        for (const auto &node : m_nodes) {
            result += node->as_string() + ", ";
        }
        result += ")";
        return result;
    }

private:
    std::vector<Node::Ptr> m_nodes;
};

class ArgList : public Node {
public:
    explicit ArgList(const Node::Ptr &node) : Node(0) {
        m_args.push_back(node);
    }

    ArgList(const Node::Ptr &first, const Node::Ptr &rest) : Node(0) {
        m_args.push_back(first);
        auto rest_list = std::dynamic_pointer_cast<ArgList>(rest);
        if (rest_list) {
            m_args.insert(m_args.end(), rest_list->m_args.begin(), rest_list->m_args.end());
        }
    }

    std::string as_string() const override {
        std::string result = "ArgList(";
        for (const auto &arg : m_args) {
            result += arg->as_string() + ", ";
        }
        result += ")";
        return result;
    }

private:
    std::vector<Node::Ptr> m_args;
};



}  // namespace ast

#endif  // KIRAZ_AST_KEYWORD_H
