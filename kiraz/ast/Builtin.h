#ifndef BUILTIN_H_
#define BUILTIN_H_

#include <kiraz/Compiler.h>
#include <kiraz/Node.h>

#define LOGIC 34
namespace ast {
// built-in logic funcs
class LogicOp : public Node {
public:
    // and & or ctor
    LogicOp(std::string op_name, const Node::Ptr &lhs, const Node::Ptr &rhs, const Node::Ptr &rtype)
            : Node(34), m_op_name(op_name), m_lhs(lhs), m_rhs(rhs), m_rtype(rtype) {
        assert(lhs);
        assert(rhs);
        assert(m_rtype);
    }

    // not ctor
    LogicOp(std::string op_name, const Node::Ptr &rhs, const Node::Ptr &rtype)
            : Node(34), m_op_name(op_name), m_lhs(nullptr), m_rhs(rhs), m_rtype(rtype) {
        assert(rhs);
        assert(m_rtype);
    }

    std::string as_string() const override {
        return fmt::format("LogicOp({}, {}, {}, {})", m_op_name,
                m_op_name == "not" ? "" : m_lhs->as_string(), m_rhs->as_string(),
                m_rtype->as_string());
    }

private:
    std::string m_op_name;
    Node::Ptr m_lhs;
    Node::Ptr m_rhs;
    Node::Ptr m_rtype;
};

}

#endif // BUILTIN_H_
