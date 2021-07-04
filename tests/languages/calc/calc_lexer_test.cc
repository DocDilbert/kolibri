#include "lexer/lexer.h"

#include <gtest/gtest.h>

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "languages/calc/calc_lexer.h"
#include "languages/calc/calc_parser.h"
#include "token_out.h"

using namespace languages::calc;
using namespace base;
using namespace std;
using namespace lexer;

#define NEXT_TOKEN_STRING_EQ(__it__, __equal_to__) \
  {                                                \
    stringstream ss;                               \
    auto token = *(__it__++);                      \
    ss << token;                                   \
    EXPECT_EQ(ss.str(), __equal_to__);             \
  }

#define NEXT_TOKEN_ID_EQ(__it__, __equal_to__) \
  {                                            \
    auto token = *(__it__++);                  \
    EXPECT_EQ(token.GetId(), __equal_to__);    \
  }

TEST(CalcLexerTest, Lexer_Empty) {
  stringstream ss;
  const char* line = "";
  CalcLexer lexer(line, 0);
  auto it = lexer.begin();
  EXPECT_EQ(it, lexer.end());
  NEXT_TOKEN_STRING_EQ(it, "Token(ENDOFFILE)");
}

TEST(CalcLexerTest, Lexer_Empty_With_Whitespaces) {
  stringstream ss;
  const char* line = "   ";
  CalcLexer lexer(line, strlen(line));
  auto it = lexer.begin();
  EXPECT_EQ(it, lexer.end());
  NEXT_TOKEN_STRING_EQ(it, "Token(ENDOFFILE)");
}

TEST(CalcLexerTest, Lexer_Integer_Delimited_By_Null) {
  stringstream ss;
  const char* line = "1\0002\0003";
  CalcLexer lexer(line, 5);
  auto it = lexer.begin();
  NEXT_TOKEN_STRING_EQ(it, "Token(INTEGER, 1)");
  NEXT_TOKEN_ID_EQ(it, CalcTokenId::kNullterm);
  NEXT_TOKEN_STRING_EQ(it, "Token(INTEGER, 2)");
  NEXT_TOKEN_ID_EQ(it, CalcTokenId::kNullterm);
  NEXT_TOKEN_STRING_EQ(it, "Token(INTEGER, 3)");
  EXPECT_EQ(it, lexer.end());
  NEXT_TOKEN_STRING_EQ(it, "Token(ENDOFFILE)");
}

TEST(CalcLexerTest, Lexer_Integer_OneDigit) {
  stringstream ss;
  const char* line = "1";

  CalcLexer lexer(line, strlen(line));
  auto it = lexer.begin();
  NEXT_TOKEN_STRING_EQ(it, "Token(INTEGER, 1)");
  EXPECT_EQ(it, lexer.end());
  NEXT_TOKEN_STRING_EQ(it, "Token(ENDOFFILE)");
}

TEST(CalcLexerTest, Lexer_Equation_WithParenthesis) {
  stringstream ss;
  const char* line = "(1+2)";

  CalcLexer lexer(line, strlen(line));
  auto it = lexer.begin();
  NEXT_TOKEN_STRING_EQ(it, "Token(LPARENS, ()");
  NEXT_TOKEN_STRING_EQ(it, "Token(INTEGER, 1)");
  NEXT_TOKEN_STRING_EQ(it, "Token(PLUS, +)");
  NEXT_TOKEN_STRING_EQ(it, "Token(INTEGER, 2)");
  NEXT_TOKEN_STRING_EQ(it, "Token(RPARENS, ))");
  EXPECT_EQ(it, lexer.end());
  NEXT_TOKEN_STRING_EQ(it, "Token(ENDOFFILE)");
}

TEST(CalcLexerTest, Lexer_Atomic_Plus) {
  stringstream ss;
  const char* line = "+";

  CalcLexer lexer(line, strlen(line));
  auto it = lexer.begin();
  NEXT_TOKEN_STRING_EQ(it, "Token(PLUS, +)");
  EXPECT_EQ(it, lexer.end());
  NEXT_TOKEN_STRING_EQ(it, "Token(ENDOFFILE)");
}

TEST(CalcLexerTest, Lexer_Atomic_Minus) {
  stringstream ss;
  const char* line = "-";

  CalcLexer lexer(line, strlen(line));
  auto it = lexer.begin();
  NEXT_TOKEN_STRING_EQ(it, "Token(MINUS, -)");
  EXPECT_EQ(it, lexer.end());
  NEXT_TOKEN_STRING_EQ(it, "Token(ENDOFFILE)");
}

TEST(CalcLexerTest, Lexer_Equation_3_p_2) {
  stringstream ss;
  const char* line = "3+2";

  CalcLexer lexer(line, strlen(line));
  auto it = lexer.begin();
  NEXT_TOKEN_STRING_EQ(it, "Token(INTEGER, 3)");
  NEXT_TOKEN_STRING_EQ(it, "Token(PLUS, +)");
  NEXT_TOKEN_STRING_EQ(it, "Token(INTEGER, 2)");
  EXPECT_EQ(it, lexer.end());
  NEXT_TOKEN_STRING_EQ(it, "Token(ENDOFFILE)");
}

TEST(CalcLexerTest, Lexer_Equation_33_p_237) {
  stringstream ss;
  const char* line = "33+237";

  CalcLexer lexer(line, strlen(line));
  auto it = lexer.begin();
  NEXT_TOKEN_STRING_EQ(it, "Token(INTEGER, 33)");
  NEXT_TOKEN_STRING_EQ(it, "Token(PLUS, +)");
  NEXT_TOKEN_STRING_EQ(it, "Token(INTEGER, 237)");
  EXPECT_EQ(it, lexer.end());
  NEXT_TOKEN_STRING_EQ(it, "Token(ENDOFFILE)");
}

TEST(CalcLexerTest, Lexer_Equation_33_p_237_With_Spaces) {
  stringstream ss;
  const char* line = "  33 + 237  ";

  CalcLexer lexer(line, strlen(line));
  auto it = lexer.begin();
  NEXT_TOKEN_STRING_EQ(it, "Token(INTEGER, 33)");
  NEXT_TOKEN_STRING_EQ(it, "Token(PLUS, +)");
  NEXT_TOKEN_STRING_EQ(it, "Token(INTEGER, 237)");
  EXPECT_EQ(it, lexer.end());
  NEXT_TOKEN_STRING_EQ(it, "Token(ENDOFFILE)");
}

TEST(CalcLexerTest, Lexer_Two_Integers_With_Space) {
  stringstream ss;
  const char* line = "33 237";

  CalcLexer lexer(line, strlen(line));
  auto it = lexer.begin();
  NEXT_TOKEN_STRING_EQ(it, "Token(INTEGER, 33)");
  NEXT_TOKEN_STRING_EQ(it, "Token(INTEGER, 237)");
  EXPECT_EQ(it, lexer.end());
  NEXT_TOKEN_STRING_EQ(it, "Token(ENDOFFILE)");
}

TEST(CalcLexerTest, Lexer_Equation_33_p_237_With_Unkown_Char) {
  stringstream ss;
  const char* line = "33\005+237";

  CalcLexer lexer(line, strlen(line));
  auto it = lexer.begin();
  NEXT_TOKEN_STRING_EQ(it, "Token(INTEGER, 33)");
  NEXT_TOKEN_STRING_EQ(it, "Token(UNKNOWN, \005)");
  NEXT_TOKEN_STRING_EQ(it, "Token(PLUS, +)");
  NEXT_TOKEN_STRING_EQ(it, "Token(INTEGER, 237)");
  EXPECT_EQ(it, lexer.end());
  NEXT_TOKEN_STRING_EQ(it, "Token(ENDOFFILE)");
}

TEST(CalcLexerTest, Lexer_Equation_Iterator_Compare) {
  stringstream ss;
  const char* line = "33+237";

  CalcLexer lexer1(line, strlen(line));
  CalcLexer lexer2(line, strlen(line));
  auto it1 = lexer1.begin();
  auto it2 = lexer2.begin();
  EXPECT_EQ(it1, it2);
  NEXT_TOKEN_STRING_EQ(it1, "Token(INTEGER, 33)");
  EXPECT_NE(it1, it2);
  NEXT_TOKEN_STRING_EQ(it2, "Token(INTEGER, 33)");
  EXPECT_EQ(it1, it2);
  NEXT_TOKEN_STRING_EQ(it1, "Token(PLUS, +)");
  EXPECT_NE(it1, it2);
  NEXT_TOKEN_STRING_EQ(it2, "Token(PLUS, +)");
  EXPECT_EQ(it1, it2);
  NEXT_TOKEN_STRING_EQ(it1, "Token(INTEGER, 237)");
  EXPECT_NE(it1, it2);
  NEXT_TOKEN_STRING_EQ(it2, "Token(INTEGER, 237)");
  EXPECT_EQ(it1, it2);
}

TEST(CalcLexerTest, Lexer_Equation_Iterator_Compare_With_End) {
  stringstream ss;
  const char* line = "33+237\0";

  CalcLexer lexer(line, strlen(line) + 1);
  auto it = lexer.begin();
  auto end = lexer.end();

  NEXT_TOKEN_STRING_EQ(it, "Token(INTEGER, 33)");
  EXPECT_NE(it, end);

  NEXT_TOKEN_STRING_EQ(it, "Token(PLUS, +)");
  EXPECT_NE(it, end);

  NEXT_TOKEN_STRING_EQ(it, "Token(INTEGER, 237)");
  EXPECT_NE(it, end);

  NEXT_TOKEN_ID_EQ(it, CalcTokenId::kNullterm);
  EXPECT_EQ(it, end);
}


