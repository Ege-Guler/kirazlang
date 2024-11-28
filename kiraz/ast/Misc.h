#ifndef KIRAZ_AST_MISC_H
#define KIRAZ_AST_MISC_H

#include <cassert>
#include <kiraz/Node.h>
#include <vector>
#include <kiraz/Compiler.h>
namespace ast {

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

    const std::vector<Node::Ptr> &get_nodes() const {
        return m_nodes;
    }


private:
std::vector<Node::Ptr> m_nodes;


};


class Module : public Node {
public:
    explicit Module(const Node::Ptr &stmt_list) : Node(0), m_stmt_list(stmt_list) {
        
    }

    std::string as_string() const override {
        if (m_stmt_list) {
            return fmt::format("Module([{}])", m_stmt_list->as_string());
        }
        else {
            return "Module([])";
        }
    }

    Ptr compute_stmt_type(SymbolTable &st) override{
        set_cur_symtab(st.get_cur_symtab());
        add_to_symtab_ordered(st);

    if (m_stmt_list) {
        auto node_list = std::dynamic_pointer_cast<ast::NodeList>(m_stmt_list);
        if (!node_list) {
            return set_error("Invalid NodeList in Module");
        }

        for (const auto &node : node_list->get_nodes()) {
            if (node) {
                if (auto error = node->compute_stmt_type(st)) {
                    return error;
                }
            }
        }
    }
        return nullptr;
    }

    Ptr add_to_symtab_ordered(SymbolTable &st) override{
        
        auto moduel_scope = st.enter_scope(ScopeType::Module, shared_from_this());

        st.add_symbol("Boolean", shared_from_this());
        st.add_symbol("fun", shared_from_this());
        st.add_symbol("class", shared_from_this());
        st.add_symbol("Integer64", shared_from_this());
        st.add_symbol("String", shared_from_this());
        st.add_symbol("Void", shared_from_this());
        st.add_symbol("void", shared_from_this());
        st.add_symbol("and", shared_from_this());
        st.add_symbol("or", shared_from_this());
        st.add_symbol("not", shared_from_this());
        st.add_symbol("let", shared_from_this());

        return nullptr;
    }

private:
    Node::Ptr m_stmt_list;
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

class CallList : public Node {
public:
    
    explicit CallList(const Node::Ptr &arg) : Node(0) {
        m_args.push_back(arg);
    }

    CallList(const Node::Ptr &first, const Node::Ptr &rest) : Node(0) {
        m_args.push_back(first);
        if (rest) {
            auto restList = std::dynamic_pointer_cast<CallList>(rest);
            if (restList) {
                m_args.insert(m_args.end(), restList->m_args.begin(), restList->m_args.end());
            } else {
                m_args.push_back(rest);
            }
        }
    }

    std::string as_string() const override {
        std::string result = "FuncArgs([";
        for (size_t i = 0; i < m_args.size(); ++i) {
            result += m_args[i]->as_string();
            if (i < m_args.size() - 1) {
                result += ", ";
            }
        }
        result += "])";
        return result;
    }

private:
    std::vector<Node::Ptr> m_args;
};


class Call : public Node {
public:
    Call(const Node::Ptr &callee, const Node::Ptr &args)
        : Node(0), m_callee(callee), m_args(args) {
        assert(callee);
    }

    std::string as_string() const override {
        return fmt::format("Call(n={}, a={})",
                           m_callee->as_string(),
                           m_args ? m_args->as_string() : "[]");
    }

private:
    Node::Ptr m_callee;
    Node::Ptr m_args;
};

} // namespace ast

#endif // KIRAZ_AST_MISC_H
