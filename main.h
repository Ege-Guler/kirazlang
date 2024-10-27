
#pragma once

#include <memory>

class Node;

extern "C" int yylex(void);
#define YY_DECL int yylex(void)
#define YYSTYPE std::shared_ptr<Node>

#include "parser.hpp"
