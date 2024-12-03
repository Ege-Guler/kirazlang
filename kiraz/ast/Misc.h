#ifndef KIRAZ_AST_MISC_H
#define KIRAZ_AST_MISC_H

#include <cassert>
#include <kiraz/Compiler.h>
#include <kiraz/Node.h>
#include <vector>

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

    const std::vector<Node::Ptr> &get_nodes() const { return m_nodes; }

private:
    std::vector<Node::Ptr> m_nodes;
};

class Module : public Node {
public:
    explicit Module(const Node::Ptr &stmt_list) : Node(0), m_stmt_list(stmt_list) {}

    std::string as_string() const override {
        if (m_stmt_list) {
            return fmt::format("Module([{}])", m_stmt_list->as_string());
        }
        else {
            return "Module([])";
        }
    }

    Ptr compute_stmt_type(SymbolTable &st) override {
        set_cur_symtab(st.get_cur_symtab());

        if (m_stmt_list) {

            auto scope = st.enter_scope(ScopeType::Module, shared_from_this());

            auto node_list = std::dynamic_pointer_cast<ast::NodeList>(m_stmt_list);

            m_symtab = std::make_unique<SymbolTable>(ScopeType::Module);

            if (! node_list) {
                return set_error("Invalid NodeList in Module");
            }

            for (const auto &node : node_list->get_nodes()) {
                if (node) {
                    if (auto ret = node->add_to_symtab_forward(st)) {
                        return ret;
                    }

                    if (auto ret = node->add_to_symtab_forward(*m_symtab)) {
                        return ret;
                    }
                }
            }

            for (const auto &node : node_list->get_nodes()) {
                if (node) {
                    if (auto ret = node->add_to_symtab_ordered(st)) {
                        return ret;
                    }

                    if (auto ret = node->add_to_symtab_ordered(*m_symtab)) {
                        return ret;
                    }

                    if (auto ret = node->compute_stmt_type(st)) {
                        return ret;
                    }
                }
            }
        }
        return nullptr;
    }

private:
    Node::Ptr m_stmt_list;
    std::unique_ptr<SymbolTable> m_symtab;
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

    const std::vector<std::pair<Node::Ptr, Node::Ptr>> &get_args() const { return m_args; }

private:
    std::vector<std::pair<Node::Ptr, Node::Ptr>> m_args;
};

class CallList : public Node {
public:
    explicit CallList(const Node::Ptr &arg) : Node(0) { m_args.push_back(arg); }

    CallList(const Node::Ptr &first, const Node::Ptr &rest) : Node(0) {
        m_args.push_back(first);
        if (rest) {
            auto restList = std::dynamic_pointer_cast<CallList>(rest);
            if (restList) {
                m_args.insert(m_args.end(), restList->m_args.begin(), restList->m_args.end());
            }
            else {
                m_args.push_back(rest);
            }
        }
    }

    const std::vector<Node::Ptr> &get_args() const { return m_args; }

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
    Call(const Node::Ptr &callee, const Node::Ptr &args) : Node(0), m_callee(callee), m_args(args) {
        assert(callee);
    }

    std::string as_string() const override {
        return fmt::format(
                "Call(n={}, a={})", m_callee->as_string(), m_args ? m_args->as_string() : "[]");
    }

    Ptr compute_stmt_type(SymbolTable &st) override {
        Node::compute_stmt_type(st);

        if (m_callee) {
            if (auto ret = m_callee->compute_stmt_type(st)) {
                return ret;
            }
        }

        if (m_args) {
            auto arg_list = std::dynamic_pointer_cast<ast::CallList>(m_args);
            if (arg_list == nullptr) {
                return set_error("Invalid CallList in Call");
            }
            for (const auto &arg : arg_list->get_args()) {
                if (auto ret = arg->compute_stmt_type(st)) {
                    return ret;
                }
            }
        }

        return nullptr;
    }

    std::string get_type(SymbolTable &st) const override {

        auto callee = std::dynamic_pointer_cast<ast::Identifier>(m_callee);

        auto func_entry = st.get_symbol(callee->get_value());

        auto func_ptr = func_entry.stmt;

        return func_ptr->get_type(st);
    }

private:
    Node::Ptr m_callee;
    Node::Ptr m_args;
};

} // namespace ast

#endif // KIRAZ_AST_MISC_H
