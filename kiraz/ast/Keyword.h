#ifndef KIRAZ_AST_KEYWORD_H
#define KIRAZ_AST_KEYWORD_H

#include <cassert>
#include <cctype>
#include <iostream>
#include <kiraz/Compiler.h>
#include <kiraz/Node.h>
#include <kiraz/ast/Misc.h>
#include <set>
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
    
    Node::Ptr get_identifier() const { return m_identifier; }
    Node::Ptr get_type() const { return m_type; }
    Node::Ptr get_initial_val() const { return m_initial_val; }

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
        }
        else {
            result += ", a=[]";
        }

        result += fmt::format(", r={}", m_rtype->as_string());

        if (m_scope) {
            result += fmt::format(", s=[{}])", m_scope->as_string());
        }
        else {
            result += ", s=[])";
        }

        return result;
    }

    Ptr add_to_symtab_forward(SymbolTable &st) override {

        auto func_name = std::dynamic_pointer_cast<ast::Identifier>(m_name);
        if (st.get_symbol(func_name->get_value())) {
            return set_error(
                    fmt::format("Identifier '{}' is already in symtab", func_name->get_value()));
        }

        // Use it for KwClass

        // if (std::islower(func_name->get_value()[0])) {
        //     return set_error(
        //             fmt::format("Function name '{}' can not start with an lowercase letter",
        //                     func_name->get_value()));
        // }
        st.add_symbol(func_name->get_value(), shared_from_this());

        return nullptr;
    }

    Ptr compute_stmt_type(SymbolTable &st) override {

        if (auto ret = Node::compute_stmt_type(st)) {
            return ret;
        }

        auto func_name = std::dynamic_pointer_cast<ast::Identifier>(m_name);
        auto rtype_id = std::dynamic_pointer_cast<ast::Identifier>(m_rtype);
        auto arg_list = std::dynamic_pointer_cast<ast::ArgList>(m_args);

        if (! st.get_symbol(rtype_id->get_value())) {
            return set_error(fmt::format("Return type '{}' of function '{}' is not found",
                    rtype_id->get_value(), func_name->get_value()));
        }

        // compute_stmnt_type for nodes and args
        SymbolTable::ScopeRef scope_ref = st.enter_scope(ScopeType::Func, shared_from_this());

        if (m_args && arg_list) {
            std::set<std::string> arg_names;

            for (const auto &[identifier, type] : arg_list->get_args()) {

                auto arg_name = std::dynamic_pointer_cast<ast::Identifier>(identifier);

                auto type_name = std::dynamic_pointer_cast<ast::Identifier>(type);

                if (! arg_name) {
                    return set_error("Argument name is not a valid identifier");
                }

                if (! st.get_symbol(type_name->get_value())) {
                    return set_error(fmt::format("Identifier '{}' in type of argument '{}' in "
                                                 "function '{}' is not found",
                            type_name->get_value(), arg_name->get_value(), func_name->get_value()));
                }

                if (! arg_names.insert(arg_name->get_value()).second) {
                    return set_error(
                            fmt::format("Identifier '{}' in argument list of function '{}' "
                                        "is already in symtab",
                                    arg_name->get_value(), func_name->get_value()));
                }

                if (arg_name->get_value() == func_name->get_value()) {
                    return set_error(
                            fmt::format("Identifier '{}' in argument list of function '{}' "
                                        "is already in symtab",
                                    arg_name->get_value(), func_name->get_value()));
                }

                st.add_symbol(arg_name->get_value(), identifier);
            }
        }

        // if there is a function body, compute its type
        if (m_scope) {
            auto node_list = std::dynamic_pointer_cast<ast::NodeList>(m_scope);
            if (! node_list) {
                return set_error("Invalid NodeList in Function");
            }

            for (const auto &node : node_list->get_nodes()) {
                if (node) {

                    if (! node->is_identifier()) {
                        if (auto ret = node->add_to_symtab_forward(st)) {
                            return ret;
                        }
                    }

                    if (auto ret = node->compute_stmt_type(st)) {
                        return ret;
                    }
                }
            }
        }

        return nullptr;
    }
    
    Node::Ptr get_name() const { return m_name; }
    Node::Ptr get_args() const { return m_args; }
    Node::Ptr get_rtype() const { return m_rtype; }
    Node::Ptr get_scope() const { return m_scope; }

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
    
    Node::Ptr get_identifier() const { return m_identifier; }
    
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
        std::string else_str;

        if (m_else_block && dynamic_cast<KwIf *>(m_else_block.get())) {
            else_str = m_else_block->as_string();
        }
        else {
            else_str = fmt::format("[{}]", m_else_block ? m_else_block->as_string() : "");
        }

        return fmt::format("If(?={}, then=[{}], else={})", m_condition->as_string(),
                m_then_block ? m_then_block->as_string() : "", else_str);
    }

    Ptr compute_stmt_type(SymbolTable &st) override {
        set_cur_symtab(st.get_cur_symtab());
        if (st.get_scope_type() == ScopeType::Module) {
            return set_error("Misplaced if statement");
        }
        return nullptr;
    }
    
    Node::Ptr get_condition() const { return m_condition; }
    Node::Ptr get_then_block() const { return m_then_block; }
    Node::Ptr get_else_block() const { return m_else_block; }

private:
    Node::Ptr m_condition;
    Node::Ptr m_then_block;
    Node::Ptr m_else_block;
};

class KwClass : public Node {
public:
    KwClass(const Node::Ptr &name, const Node::Ptr &stmt_list, const Node::Ptr &base_name = nullptr)
            : Node(KW_CLASS)
            , m_name(name)
            , m_stmt_list(stmt_list ? stmt_list : nullptr)
            , m_base_name(base_name) {
        assert(name);
    }

    std::string as_string() const override {
        if (m_base_name) {
            return fmt::format("Class(n={}, base={}, s=[{}])", m_name->as_string(),
                    m_base_name->as_string(), m_stmt_list ? m_stmt_list->as_string() : "");
        }
        else {
            return fmt::format("Class(n={}, s=[{}])", m_name->as_string(),
                    m_stmt_list ? m_stmt_list->as_string() : "");
        }
    }

    Ptr compute_stmt_type(SymbolTable &st) override {

        auto iden = std::dynamic_pointer_cast<ast::Identifier>(m_name);
        if (std::islower(iden->get_value()[0])) {
            return set_error(fmt::format(
                    "Class name '{}' cannot start with a lowercase letter", iden->get_value()));
        }

        if (m_base_name) {
            auto base_iden = std::dynamic_pointer_cast<ast::Identifier>(m_base_name);
            if (std::islower(base_iden->get_value()[0])) {
                return set_error(
                        fmt::format("Base class name '{}' cannot start with a lowercase letter",
                                base_iden->get_value()));
            }
        }

        return nullptr;
    }
    
    Node::Ptr get_name() const { return m_name; }
    Node::Ptr get_stmt_list() const { return m_stmt_list; }
    Node::Ptr get_base_name() const { return m_base_name; }
    
private:
    Node::Ptr m_name;
    Node::Ptr m_stmt_list;
    Node::Ptr m_base_name;
    std::unique_ptr<SymbolTable> m_symtab;
};

class KwWhile : public Node {
public:
    KwWhile(const Node::Ptr &condition, const Node::Ptr &stmt_list)
            : Node(KW_WHILE), m_condition(condition), m_stmt_list(stmt_list ? stmt_list : nullptr) {
        assert(condition);
    }
    std::string as_string() const override {
        return fmt::format("While(?={}, repeat=[{}])", m_condition->as_string(),
                m_stmt_list ? m_stmt_list->as_string() : "");
    }

    Ptr compute_stmt_type(SymbolTable &st) override {
        set_cur_symtab(st.get_cur_symtab());
        if (st.get_scope_type() == ScopeType::Module) {
            return set_error("Misplaced while statement");
        }

        return nullptr;
    }
    
    Node::Ptr get_condition() const { return m_condition; }
    Node::Ptr get_stmt_list() const { return m_stmt_list; }
    
private:
    Node::Ptr m_condition;
    Node::Ptr m_stmt_list;
};

class KwReturn : public Node {
public:
    explicit KwReturn(const Node::Ptr &value) : Node(KW_RETURN), m_value(value) { assert(value); }

    std::string as_string() const override {
        return fmt::format("Return({})", m_value->as_string());
    }

    Ptr compute_stmt_type(SymbolTable &st) override {
        set_cur_symtab(st.get_cur_symtab());
        if (st.get_scope_type() == ScopeType::Module) {
            return set_error("Misplaced return statement");
        }
        return nullptr;
    }

    Node::Ptr get_value() const { return m_value; }
private:
    Node::Ptr m_value;
};

} // namespace ast

#endif // KIRAZ_AST_KEYWORD_H
