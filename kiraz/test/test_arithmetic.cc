
#include <gtest/gtest.h>

#include <lexer.h>
#include <main.h>

#include <kiraz/Node.h>

extern std::shared_ptr<Token> curtoken;

struct ParserFixture : public testing::Test {
    YY_BUFFER_STATE buffer = nullptr;

    void SetUp() override {
        Node::reset_root();
        // yydebug = 1; // uncomment to your heart's content
    }

    void TearDown() override {
        // Tear down code after each test, even if assertions fail.
        // This will be executed even in the face of assertion failures.
        if (buffer) {
            yy_delete_buffer(buffer);
            buffer = nullptr;
        }

        yydebug = 0;
        Token::colno = 0;
        curtoken.reset();
    }

    void verify_root(const std::string &code, const std::string &ast) {
        buffer = yy_scan_string(code.data());

        /* perform */
        yyparse();

        /* verify */
        ASSERT_TRUE(Node::current_root());
        auto root = Node::current_root();
        ASSERT_EQ(fmt::format("{}", root->as_string()), ast);
    }

    void verify_no_root(const std::string &code) {
        buffer = yy_scan_string(code.data());

        /* perform */
        yyparse();

        /* verify */
        ASSERT_FALSE(Node::current_root());
    }
};

TEST_F(ParserFixture, empty) {
    verify_no_root("");
}

TEST_F(ParserFixture, invalid_char) {
    verify_no_root("=");
}

TEST_F(ParserFixture, plus) {
    verify_root("1+2", "Add(Integer(1),Integer(2))");
}

TEST_F(ParserFixture, plus_s) {
    verify_root("1+2 ", "Add(Integer(1),Integer(2))");
}

TEST_F(ParserFixture, plus_sb) {
    verify_root("1 +2", "Add(Integer(1),Integer(2))");
}

TEST_F(ParserFixture, plus_ss) {
    verify_root("1 + 2", "Add(Integer(1),Integer(2))");
}

TEST_F(ParserFixture, plus_sa) {
    verify_root("1+ 2", "Add(Integer(1),Integer(2))");
}           

TEST_F(ParserFixture, plus_n) {
    verify_root("1+\n2", "Add(Integer(1),Integer(2))");
}

TEST_F(ParserFixture, plus_t) {
    verify_root("1+\t2", "Add(Integer(1),Integer(2))");
}

TEST_F(ParserFixture, minus) {
    verify_root("1-2", "Sub(Integer(1),Integer(2))");
}

TEST_F(ParserFixture, mult) {
    verify_root("1*2", "Mult(Integer(1),Integer(2))");
}

TEST_F(ParserFixture, divf) {
    verify_root("1/2", "DivF(Integer(1),Integer(2))");
}

TEST_F(ParserFixture, muladd) {
    verify_root("1+2*3", "Add(Integer(1),Mult(Integer(2),Integer(3)))");
}

TEST_F(ParserFixture, muladd_paren) {
    verify_root("1+(2*3)", "Add(Integer(1),Mult(Integer(2),Integer(3)))");
}

TEST_F(ParserFixture, addmul_paren) {
    verify_root("(1+2)*3", "Mult(Add(Integer(1),Integer(2)),Integer(3))");
}
