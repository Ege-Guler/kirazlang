
#include "Literal.h"

#include <cassert>
#include <kiraz/token/Literal.h>

namespace ast {
Integer::Integer(Token::Ptr t) : Node(L_INTEGER) {
    assert(t->get_id() == L_INTEGER);
    auto token_int = std::static_pointer_cast<const token::Integer>(t);
    auto base = token_int->get_base();

    try {
        m_value = std::stoll(token_int->get_value(), nullptr, base);
        m_base = base;
    }
    catch (const std::exception &e) {
        // TODO Mark this node as invalid
    }
}

Identifier::Identifier(Token::Ptr t) : Node(IDENTIFIER) {
    assert(t->get_id() == IDENTIFIER); 
    auto token_identifier = std::static_pointer_cast<const token::Identifier>(t); 
    m_value = token_identifier->get_value(); 
}

String::String(Token::Ptr t): Node(L_STRING) {
    assert(t->get_id() == L_STRING);
    auto token_identifier = std::static_pointer_cast<const token::String>(t); 
    m_value = token_identifier->get_value();
}

}
