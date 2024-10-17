#ifndef KIRAZ_TOKEN_H
#define KIRAZ_TOKEN_H

#include "main.h"

#include <fmt/format.h>

class Token {
public:
    using Ptr = std::shared_ptr<Token>;
    using Cptr = std::shared_ptr<const Token>;

    Token(int id) : m_id(id) {}
    virtual ~Token();

    static auto New() {}

    virtual std::string as_string() const = 0;
    void print() { fmt::print("{}\n", as_string()); }

    template <typename T, typename... Args>
    static auto New(Args &&...args) {
        return std::make_shared<T>(std::forward<Args>(args)...);
    }

    static int colno;

    auto get_id() const { return m_id; }

private:
    int m_id;
};

class Rejected : public Token {
public:
    Rejected(const char *text) : Token(REJECTED), m_text(text) {}
    std::string as_string() const override { return fmt::format("REJECTED({})", m_text); }

private:
    std::string m_text;
};

#endif // KIRAZ_TOKEN_H
