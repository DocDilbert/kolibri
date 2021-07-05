#include <gmock/gmock.h>  // Brings in Google Mock.
#include <gtest/gtest.h>

#include "languages/ast.h"
#include "languages/ast_factory.h"
#include "languages/ast_types.h"
#include "languages/calc/calc_interpreter.h"
#include "languages/calc/calc_lexer.h"
#include "languages/calc/calc_parser.h"
#include "languages/calc/calc_parser_factory.h"
#include "lexer/lexer.h"
#include "parser/parser.h"

using namespace base;
using namespace languages;
using namespace languages::calc;
using namespace parser;
using namespace std;
using namespace lexer;

using ::testing::_;
using ::testing::Ref;
using ::testing::Return;

TEST(CalcIntegrationTest, Num) {
  stringstream ss;
  const char* line = "1";

  CalcLexer lexer(line, strlen(line));
  AstFactory<std::shared_ptr<Ast<MakeShared, CalcToken>>, CalcToken> ast_factory;
  CalcParserFactory parser_factory(ast_factory);
  CalcParser parser(parser_factory);

  auto parser_res = parser.Expr(lexer.begin(), lexer.end());
  CalcInterpreter<MakeShared, CalcToken> CalcInterpreter;

  auto res = CalcInterpreter.Interpret(parser_res.node);
  EXPECT_EQ("1", res);
}

TEST(CalcIntegrationTest, Minus_Num) {
  stringstream ss;
  const char* line = "-1";

  CalcLexer lexer(line, strlen(line));
  AstFactory<std::shared_ptr<Ast<MakeShared, CalcToken>>, CalcToken> ast_factory;
  CalcParserFactory parser_factory(ast_factory);
  CalcParser parser(parser_factory);

  auto parser_res = parser.Expr(lexer.begin(), lexer.end());
  CalcInterpreter<MakeShared, CalcToken> CalcInterpreter;

  auto res = CalcInterpreter.Interpret(parser_res.node);
  EXPECT_EQ("-1", res);
}

TEST(CalcIntegrationTest, Equation_1_plus_2) {
  stringstream ss;
  const char* line = "1+2";

  CalcLexer lexer(line, strlen(line));
  AstFactory<std::shared_ptr<Ast<MakeShared, CalcToken>>, CalcToken> ast_factory;
  CalcParserFactory parser_factory(ast_factory);
  CalcParser parser(parser_factory);

  auto parser_res = parser.Expr(lexer.begin(), lexer.end());
  CalcInterpreter<MakeShared, CalcToken> CalcInterpreter;

  auto res = CalcInterpreter.Interpret(parser_res.node);
  EXPECT_EQ("3", res);
}

TEST(CalcIntegrationTest, Equation_m_1_p_2) {
  stringstream ss;
  const char* line = "-1+2";

  CalcLexer lexer(line, strlen(line));
  AstFactory<std::shared_ptr<Ast<MakeShared, CalcToken>>, CalcToken> ast_factory;
  CalcParserFactory parser_factory(ast_factory);
  CalcParser parser(parser_factory);

  auto parser_res = parser.Expr(lexer.begin(), lexer.end());
  CalcInterpreter<MakeShared, CalcToken> CalcInterpreter;

  auto res = CalcInterpreter.Interpret(parser_res.node);
  EXPECT_EQ("1", res);
}

TEST(CalcIntegrationTest, Equation_lp_1_p_2_rp) {
  stringstream ss;
  const char* line = "(((1+2)))";

  CalcLexer lexer(line, strlen(line));
  AstFactory<std::shared_ptr<Ast<MakeShared, CalcToken>>, CalcToken> ast_factory;
  CalcParserFactory parser_factory(ast_factory);
  CalcParser parser(parser_factory);

  auto parser_res = parser.Expr(lexer.begin(), lexer.end());
  CalcInterpreter<MakeShared, CalcToken> CalcInterpreter;

  auto res = CalcInterpreter.Interpret(parser_res.node);
  EXPECT_EQ("3", res);
}

TEST(CalcIntegrationTest, Equation_lp_1_p_2_rp_mul_3) {
  stringstream ss;
  const char* line = "(((1+2)))*3";

  CalcLexer lexer(line, strlen(line));
  AstFactory<std::shared_ptr<Ast<MakeShared, CalcToken>>, CalcToken> ast_factory;
  CalcParserFactory parser_factory(ast_factory);
  CalcParser parser(parser_factory);

  auto parser_res = parser.Expr(lexer.begin(), lexer.end());
  CalcInterpreter<MakeShared, CalcToken> CalcInterpreter;

  auto res = CalcInterpreter.Interpret(parser_res.node);
  EXPECT_EQ("9", res);
}

TEST(CalcIntegrationTest, Equation_lp_1_p_2_rp_mul_lp_2_p_1_rp) {
  stringstream ss;
  const char* line = "(((1+2)))*(((2+1)))";

  CalcLexer lexer(line, strlen(line));
  AstFactory<std::shared_ptr<Ast<MakeShared, CalcToken>>, CalcToken> ast_factory;
  CalcParserFactory parser_factory(ast_factory);
  CalcParser parser(parser_factory);

  auto parser_res = parser.Expr(lexer.begin(), lexer.end());
  CalcInterpreter<MakeShared, CalcToken> CalcInterpreter;

  auto res = CalcInterpreter.Interpret(parser_res.node);
  EXPECT_EQ("9", res);
}
