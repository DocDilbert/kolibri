#include "parser/parser.h"

#include <gmock/gmock.h>  // Brings in Google Mock.
#include <gtest/gtest.h>

#include <string>
#include <vector>

#include "base/token.h"
#include "languages/calc/calc_parser.h"

using namespace languages::calc;
using namespace base;
using namespace parser;
using namespace std;

using ::testing::_;
using ::testing::Return;

class MockAstTraits {
 public:
  using value_type = string;
  using reference_type = string;
  using pointer_type = string;
};

using NonTermType = std::string;
using MockToken = Token<CalcTokenId, CalcTokenIdConverter>;

template <typename T>
class MockParserFactory : public IParserFactory<T, CalcToken> {
 public:
  using nonterm_type = T;
  using term_type = MockToken;

  MOCK_METHOD0(CreateNull, nonterm_type());
  MOCK_METHOD1(CreateEmpty, nonterm_type(parser::RuleId rule_id));
  MOCK_METHOD2(CreateTerm, nonterm_type(parser::RuleId rule_id, term_type term));
  MOCK_METHOD2(CreateNonTerm, nonterm_type(parser::RuleId rule_id, nonterm_type nonterm));
  MOCK_METHOD2(CreateNonTermList, nonterm_type(parser::RuleId rule_id, std::vector<nonterm_type> statements));
  MOCK_METHOD3(CreateTermNonTermList, nonterm_type(parser::RuleId rule_id, std::vector<term_type> terms, std::vector<nonterm_type> nonterms));

  MOCK_METHOD3(CreateTermNonTerm, nonterm_type(parser::RuleId rule_id, term_type term, nonterm_type nonterm));
  MOCK_METHOD3(CreateNonTermNonTerm, nonterm_type(parser::RuleId rule_id, nonterm_type lhs, nonterm_type rhs));
  MOCK_METHOD4(CreateNonTermTermNonTerm, nonterm_type(parser::RuleId rule_id, nonterm_type lhs, term_type term, nonterm_type rhs));
};

using MockIterator = vector<MockToken>::iterator;
using MockGrammar = CalculatorGrammar<NonTermType, MockIterator>;

TEST(ParserTest, Integer) {
  vector<MockToken> input;
  input.push_back(MockToken(CalcTokenId::INTEGER, "3", 1));

  MockParserFactory<NonTermType> mock_parser_factory;
  Parser<MockGrammar> parser(mock_parser_factory);
  EXPECT_CALL(mock_parser_factory, CreateNull).WillRepeatedly(Return("NULL"));
  EXPECT_CALL(mock_parser_factory, CreateTerm(RuleId::kRule2, MockToken(CalcTokenId::INTEGER, "3", 1))).Times(1).WillOnce(Return("3"));
  auto result = parser.Expr(input.begin(), input.end());

  EXPECT_EQ(result.node, "3");
}

TEST(ParserTest, Integer_With_Brace) {
  vector<MockToken> input;
  input.push_back(MockToken(CalcTokenId::LPARENS, "(", 1));
  input.push_back(MockToken(CalcTokenId::INTEGER, "3", 1));
  input.push_back(MockToken(CalcTokenId::RPARENS, ")", 1));

  MockParserFactory<NonTermType> mock_parser_factory;
  Parser<MockGrammar> parser(mock_parser_factory);
  EXPECT_CALL(mock_parser_factory, CreateNull).WillRepeatedly(Return("NULL"));
  EXPECT_CALL(mock_parser_factory, CreateTerm(RuleId::kRule2, MockToken(CalcTokenId::INTEGER, "3", 1))).Times(1).WillOnce(Return("3"));
  auto result = parser.Expr(input.begin(), input.end());

  EXPECT_EQ(result.node, "3");
}

TEST(ParserTest, Integer_With_Double_Brace) {
  vector<MockToken> input;
  input.push_back(MockToken(CalcTokenId::LPARENS, "(", 1));
  input.push_back(MockToken(CalcTokenId::LPARENS, "(", 1));
  input.push_back(MockToken(CalcTokenId::INTEGER, "3", 1));
  input.push_back(MockToken(CalcTokenId::RPARENS, ")", 1));
  input.push_back(MockToken(CalcTokenId::RPARENS, ")", 1));

  MockParserFactory<NonTermType> mock_parser_factory;
  Parser<MockGrammar> parser(mock_parser_factory);
  EXPECT_CALL(mock_parser_factory, CreateTerm(RuleId::kRule2, MockToken(CalcTokenId::INTEGER, "3", 1))).Times(1).WillOnce(Return("3"));
  EXPECT_CALL(mock_parser_factory, CreateNull()).WillRepeatedly(Return("NULL"));
  auto result = parser.Expr(input.begin(), input.end());

  EXPECT_EQ(result.node, "3");
}

TEST(ParserTest, Equation_3_plus_2) {
  vector<MockToken> input;
  input.push_back(MockToken(CalcTokenId::INTEGER, "3", 1));
  input.push_back(MockToken(CalcTokenId::PLUS));
  input.push_back(MockToken(CalcTokenId::INTEGER, "2", 1));

  MockParserFactory<NonTermType> mock_parser_factory;
  Parser<MockGrammar> parser(mock_parser_factory);

  EXPECT_CALL(mock_parser_factory, CreateNull).WillRepeatedly(Return("NULL"));
  EXPECT_CALL(mock_parser_factory, CreateTerm(RuleId::kRule2, MockToken(CalcTokenId::INTEGER, "3", 1))).Times(1).WillOnce(Return("3"));
  EXPECT_CALL(mock_parser_factory, CreateTerm(RuleId::kRule2, MockToken(CalcTokenId::INTEGER, "2", 1))).Times(1).WillOnce(Return("2"));
  EXPECT_CALL(mock_parser_factory, CreateNonTermTermNonTerm(RuleId::kRule0, "3", MockToken(CalcTokenId::PLUS), "2")).WillOnce(Return("ADD_3_2"));

  // EXPECT_CALL(mock_parser_factory, CreateBinaryOp(BinaryOpType::ADD, "3", "2")).WillOnce(Return("ADD_3_2"));

  auto result = parser.Expr(input.begin(), input.end());

  EXPECT_EQ(result.node, "ADD_3_2");
}

TEST(ParserTest, UnaryInteger) {
  vector<MockToken> input;
  input.push_back(MockToken(CalcTokenId::MINUS, "-", 1));
  input.push_back(MockToken(CalcTokenId::INTEGER, "3", 1));

  MockParserFactory<NonTermType> mock_parser_factory;
  Parser<MockGrammar> parser(mock_parser_factory);

  EXPECT_CALL(mock_parser_factory, CreateNull).WillRepeatedly(Return("NULL"));
  EXPECT_CALL(mock_parser_factory, CreateTerm(RuleId::kRule2, MockToken(CalcTokenId::INTEGER, "3", 1))).Times(1).WillOnce(Return("3"));
  EXPECT_CALL(mock_parser_factory, CreateTermNonTerm(RuleId::kRule2, MockToken(CalcTokenId::MINUS, "-", 1), "3")).WillOnce(Return("-3"));

  auto result = parser.Expr(input.begin(), input.end());

  EXPECT_EQ(result.node, "-3");
}

TEST(ParserTest, Equation_1_plus_3) {
  vector<MockToken> input;
  input.push_back(MockToken(CalcTokenId::INTEGER, "1", 1));
  input.push_back(MockToken(CalcTokenId::PLUS));
  input.push_back(MockToken(CalcTokenId::INTEGER, "3", 1));

  MockParserFactory<NonTermType> mock_parser_factory;
  Parser<MockGrammar> parser(mock_parser_factory);
  EXPECT_CALL(mock_parser_factory, CreateNull).WillRepeatedly(Return("NULL"));
  EXPECT_CALL(mock_parser_factory, CreateTerm(RuleId::kRule2, MockToken(CalcTokenId::INTEGER, "1", 1))).Times(1).WillOnce(Return("1"));
  EXPECT_CALL(mock_parser_factory, CreateTerm(RuleId::kRule2, MockToken(CalcTokenId::INTEGER, "3", 1))).Times(1).WillOnce(Return("3"));
  EXPECT_CALL(mock_parser_factory, CreateNonTermTermNonTerm(RuleId::kRule0, "1", MockToken(CalcTokenId::PLUS), "3")).WillOnce(Return("ADD_1_3"));

  auto result = parser.Expr(input.begin(), input.end());

  EXPECT_EQ(result.node, "ADD_1_3");
}
TEST(ParserTest, Equation_1_plus_minus_3) {
  vector<MockToken> input;
  input.push_back(MockToken(CalcTokenId::INTEGER, "1", 1));
  input.push_back(MockToken(CalcTokenId::PLUS));
  input.push_back(MockToken(CalcTokenId::MINUS));
  input.push_back(MockToken(CalcTokenId::INTEGER, "3", 1));

  MockParserFactory<NonTermType> mock_parser_factory;
  Parser<MockGrammar> parser(mock_parser_factory);
  EXPECT_CALL(mock_parser_factory, CreateNull).WillRepeatedly(Return("NULL"));
  EXPECT_CALL(mock_parser_factory, CreateTerm(RuleId::kRule2, MockToken(CalcTokenId::INTEGER, "1", 1))).WillOnce(Return("1"));
  EXPECT_CALL(mock_parser_factory, CreateTerm(RuleId::kRule2, MockToken(CalcTokenId::INTEGER, "3", 1))).WillOnce(Return("3"));
  EXPECT_CALL(mock_parser_factory, CreateTermNonTerm(RuleId::kRule2, MockToken(CalcTokenId::MINUS), "3")).WillOnce(Return("-3"));
  EXPECT_CALL(mock_parser_factory, CreateNonTermTermNonTerm(RuleId::kRule0, "1", MockToken(CalcTokenId::PLUS), "-3")).WillOnce(Return("ADD_1_-3"));

  auto result = parser.Expr(input.begin(), input.end());

  EXPECT_EQ(result.node, "ADD_1_-3");
}
TEST(ParserTest, Equation_1_plus_2_plus_3) {
  vector<MockToken> input;
  input.push_back(MockToken(CalcTokenId::INTEGER, "1", 1));
  input.push_back(MockToken(CalcTokenId::PLUS));
  input.push_back(MockToken(CalcTokenId::INTEGER, "2", 1));
  input.push_back(MockToken(CalcTokenId::PLUS));
  input.push_back(MockToken(CalcTokenId::INTEGER, "3", 1));

  MockParserFactory<NonTermType> mock_parser_factory;
  Parser<MockGrammar> parser(mock_parser_factory);
  EXPECT_CALL(mock_parser_factory, CreateTerm(RuleId::kRule2, MockToken(CalcTokenId::INTEGER, "1", 1))).Times(1).WillOnce(Return("1"));
  EXPECT_CALL(mock_parser_factory, CreateTerm(RuleId::kRule2, MockToken(CalcTokenId::INTEGER, "2", 1))).Times(1).WillOnce(Return("2"));
  EXPECT_CALL(mock_parser_factory, CreateTerm(RuleId::kRule2, MockToken(CalcTokenId::INTEGER, "3", 1))).Times(1).WillOnce(Return("3"));
  EXPECT_CALL(mock_parser_factory, CreateNonTermTermNonTerm(RuleId::kRule0, "1", MockToken(CalcTokenId::PLUS), "2")).WillOnce(Return("ADD_1_2"));
  EXPECT_CALL(mock_parser_factory, CreateNonTermTermNonTerm(RuleId::kRule0, "ADD_1_2", MockToken(CalcTokenId::PLUS), "3")).WillOnce(Return("ADD_*_3"));
  EXPECT_CALL(mock_parser_factory, CreateNull()).WillRepeatedly(Return("NULL"));

  auto result = parser.Expr(input.begin(), input.end());

  EXPECT_EQ(result.node, "ADD_*_3");
}

TEST(ParserTest, Equation_1_plus_2_minus_3) {
  vector<MockToken> input;
  input.push_back(MockToken(CalcTokenId::INTEGER, "1", 1));
  input.push_back(MockToken(CalcTokenId::PLUS));
  input.push_back(MockToken(CalcTokenId::INTEGER, "2", 1));
  input.push_back(MockToken(CalcTokenId::MINUS));
  input.push_back(MockToken(CalcTokenId::INTEGER, "3", 1));

  MockParserFactory<NonTermType> mock_parser_factory;
  Parser<MockGrammar> parser(mock_parser_factory);
  EXPECT_CALL(mock_parser_factory, CreateTerm(RuleId::kRule2, MockToken(CalcTokenId::INTEGER, "1", 1))).Times(1).WillOnce(Return("1"));
  EXPECT_CALL(mock_parser_factory, CreateTerm(RuleId::kRule2, MockToken(CalcTokenId::INTEGER, "2", 1))).Times(1).WillOnce(Return("2"));
  EXPECT_CALL(mock_parser_factory, CreateTerm(RuleId::kRule2, MockToken(CalcTokenId::INTEGER, "3", 1))).Times(1).WillOnce(Return("3"));
  EXPECT_CALL(mock_parser_factory, CreateNonTermTermNonTerm(RuleId::kRule0, "1", MockToken(CalcTokenId::PLUS), "2")).WillOnce(Return("ADD_1_2"));
  EXPECT_CALL(mock_parser_factory, CreateNonTermTermNonTerm(RuleId::kRule0, "ADD_1_2", MockToken(CalcTokenId::MINUS), "3")).WillOnce(Return("SUB_*_3"));
  EXPECT_CALL(mock_parser_factory, CreateNull()).WillRepeatedly(Return("NULL"));

  auto result = parser.Expr(input.begin(), input.end());

  EXPECT_EQ(result.node, "SUB_*_3");
}

TEST(ParserTest, Equation_2_mul_2_mul_3) {
  vector<MockToken> input;
  input.push_back(MockToken(CalcTokenId::INTEGER, "2", 1));
  input.push_back(MockToken(CalcTokenId::MULTIPLY));
  input.push_back(MockToken(CalcTokenId::INTEGER, "2", 1));
  input.push_back(MockToken(CalcTokenId::MULTIPLY));
  input.push_back(MockToken(CalcTokenId::INTEGER, "3", 1));

  MockParserFactory<NonTermType> mock_parser_factory;
  Parser<MockGrammar> parser(mock_parser_factory);

  EXPECT_CALL(mock_parser_factory, CreateTerm(RuleId::kRule2, MockToken(CalcTokenId::INTEGER, "2", 1))).WillOnce(Return("2")).WillOnce(Return("2"));
  EXPECT_CALL(mock_parser_factory, CreateTerm(RuleId::kRule2, MockToken(CalcTokenId::INTEGER, "3", 1))).Times(1).WillOnce(Return("3"));
  EXPECT_CALL(mock_parser_factory, CreateNonTermTermNonTerm(RuleId::kRule1, "2", MockToken(CalcTokenId::MULTIPLY), "2")).WillOnce(Return("MUL_2_2"));
  EXPECT_CALL(mock_parser_factory, CreateNonTermTermNonTerm(RuleId::kRule1, "MUL_2_2", MockToken(CalcTokenId::MULTIPLY), "3")).WillOnce(Return("MUL_*_3"));
  EXPECT_CALL(mock_parser_factory, CreateNull()).WillRepeatedly(Return("NULL"));

  auto result = parser.Expr(input.begin(), input.end());
  EXPECT_EQ(result.node, "MUL_*_3");
}

TEST(ParserTest, Equation_2_plus_7_mul_4) {
  vector<MockToken> input;
  input.push_back(MockToken(CalcTokenId::INTEGER, "2", 1));
  input.push_back(MockToken(CalcTokenId::PLUS));
  input.push_back(MockToken(CalcTokenId::INTEGER, "7", 1));
  input.push_back(MockToken(CalcTokenId::MULTIPLY));
  input.push_back(MockToken(CalcTokenId::INTEGER, "4", 1));

  MockParserFactory<NonTermType> mock_parser_factory;
  Parser<MockGrammar> parser(mock_parser_factory);
  EXPECT_CALL(mock_parser_factory, CreateTerm(RuleId::kRule2, MockToken(CalcTokenId::INTEGER, "2", 1))).WillOnce(Return("2"));
  EXPECT_CALL(mock_parser_factory, CreateTerm(RuleId::kRule2, MockToken(CalcTokenId::INTEGER, "7", 1))).WillOnce(Return("7"));
  EXPECT_CALL(mock_parser_factory, CreateTerm(RuleId::kRule2, MockToken(CalcTokenId::INTEGER, "4", 1))).WillOnce(Return("4"));
  EXPECT_CALL(mock_parser_factory, CreateNonTermTermNonTerm(RuleId::kRule1, "7", MockToken(CalcTokenId::MULTIPLY), "4")).WillOnce(Return("MUL_7_4"));
  EXPECT_CALL(mock_parser_factory, CreateNonTermTermNonTerm(RuleId::kRule0, "2", MockToken(CalcTokenId::PLUS), "MUL_7_4")).WillOnce(Return("MUL_2_*"));
  EXPECT_CALL(mock_parser_factory, CreateNull()).WillRepeatedly(Return("NULL"));

  auto result = parser.Expr(input.begin(), input.end());
  EXPECT_EQ(result.node, "MUL_2_*");
}

TEST(ParserTest, Equation_lp_2_plus_7_rp_mul_4) {
  vector<MockToken> input;
  input.push_back(MockToken(CalcTokenId::LPARENS, "(", 1));
  input.push_back(MockToken(CalcTokenId::INTEGER, "2", 1));
  input.push_back(MockToken(CalcTokenId::PLUS));
  input.push_back(MockToken(CalcTokenId::INTEGER, "7", 1));
  input.push_back(MockToken(CalcTokenId::RPARENS, ")", 1));
  input.push_back(MockToken(CalcTokenId::MULTIPLY));
  input.push_back(MockToken(CalcTokenId::INTEGER, "4", 1));

  MockParserFactory<NonTermType> mock_parser_factory;
  Parser<MockGrammar> parser(mock_parser_factory);
  EXPECT_CALL(mock_parser_factory, CreateTerm(RuleId::kRule2, MockToken(CalcTokenId::INTEGER, "2", 1))).WillOnce(Return("2"));
  EXPECT_CALL(mock_parser_factory, CreateTerm(RuleId::kRule2, MockToken(CalcTokenId::INTEGER, "7", 1))).WillOnce(Return("7"));
  EXPECT_CALL(mock_parser_factory, CreateTerm(RuleId::kRule2, MockToken(CalcTokenId::INTEGER, "4", 1))).WillOnce(Return("4"));
  EXPECT_CALL(mock_parser_factory, CreateNonTermTermNonTerm(RuleId::kRule0, "2", MockToken(CalcTokenId::PLUS), "7")).WillOnce(Return("ADD_2_7"));
  EXPECT_CALL(mock_parser_factory, CreateNonTermTermNonTerm(RuleId::kRule1, "ADD_2_7", MockToken(CalcTokenId::MULTIPLY), "4")).WillOnce(Return("MUL_*_4"));
  EXPECT_CALL(mock_parser_factory, CreateNull()).WillRepeatedly(Return("NULL"));

  auto result = parser.Expr(input.begin(), input.end());
  EXPECT_EQ(result.node, "MUL_*_4");
}

TEST(ParserTest, Equation_lp_1_plus_2_rp_mul_lp_2_plus_1_rp) {
  vector<MockToken> input;
  input.push_back(MockToken(CalcTokenId::LPARENS, "(", 1));
  input.push_back(MockToken(CalcTokenId::INTEGER, "1", 1));
  input.push_back(MockToken(CalcTokenId::PLUS));
  input.push_back(MockToken(CalcTokenId::INTEGER, "2", 1));
  input.push_back(MockToken(CalcTokenId::RPARENS, ")", 1));
  input.push_back(MockToken(CalcTokenId::MULTIPLY));
  input.push_back(MockToken(CalcTokenId::LPARENS, "(", 1));
  input.push_back(MockToken(CalcTokenId::INTEGER, "2", 1));
  input.push_back(MockToken(CalcTokenId::PLUS));
  input.push_back(MockToken(CalcTokenId::INTEGER, "1", 1));
  input.push_back(MockToken(CalcTokenId::RPARENS, ")", 1));

  MockParserFactory<NonTermType> mock_parser_factory;
  Parser<MockGrammar> parser(mock_parser_factory);
  EXPECT_CALL(mock_parser_factory, CreateTerm(RuleId::kRule2, MockToken(CalcTokenId::INTEGER, "1", 1))).WillOnce(Return("1")).WillOnce(Return("1"));
  EXPECT_CALL(mock_parser_factory, CreateTerm(RuleId::kRule2, MockToken(CalcTokenId::INTEGER, "2", 1))).WillOnce(Return("2")).WillOnce(Return("2"));
  EXPECT_CALL(mock_parser_factory, CreateNonTermTermNonTerm(RuleId::kRule0, "1", MockToken(CalcTokenId::PLUS), "2")).WillOnce(Return("ADD_1_2"));
  EXPECT_CALL(mock_parser_factory, CreateNonTermTermNonTerm(RuleId::kRule0, "2", MockToken(CalcTokenId::PLUS), "1")).WillOnce(Return("ADD_2_1"));
  EXPECT_CALL(mock_parser_factory, CreateNonTermTermNonTerm(RuleId::kRule1, "ADD_1_2", MockToken(CalcTokenId::MULTIPLY), "ADD_2_1"))
      .WillOnce(Return("MUL_*_*"));
  EXPECT_CALL(mock_parser_factory, CreateNull()).WillRepeatedly(Return("NULL"));

  auto result = parser.Expr(input.begin(), input.end());
  EXPECT_EQ(result.node, "MUL_*_*");
}

TEST(ParserTest, Equation_2_mul_2_mul_3_div_4) {
  vector<MockToken> input;
  input.push_back(MockToken(CalcTokenId::INTEGER, "2", 1));
  input.push_back(MockToken(CalcTokenId::MULTIPLY, "*", 1));
  input.push_back(MockToken(CalcTokenId::INTEGER, "2", 1));
  input.push_back(MockToken(CalcTokenId::MULTIPLY, "*", 1));
  input.push_back(MockToken(CalcTokenId::INTEGER, "3", 1));
  input.push_back(MockToken(CalcTokenId::DIVIDE, "/", 1));
  input.push_back(MockToken(CalcTokenId::INTEGER, "4", 1));

  MockParserFactory<NonTermType> mock_parser_factory;
  Parser<MockGrammar> parser(mock_parser_factory);
  EXPECT_CALL(mock_parser_factory, CreateTerm(RuleId::kRule2, MockToken(CalcTokenId::INTEGER, "2", 1))).WillOnce(Return("2")).WillOnce(Return("2"));
  EXPECT_CALL(mock_parser_factory, CreateTerm(RuleId::kRule2, MockToken(CalcTokenId::INTEGER, "3", 1))).WillOnce(Return("3"));
  EXPECT_CALL(mock_parser_factory, CreateTerm(RuleId::kRule2, MockToken(CalcTokenId::INTEGER, "4", 1))).WillOnce(Return("4"));
  EXPECT_CALL(mock_parser_factory, CreateNonTermTermNonTerm(RuleId::kRule1, "2", MockToken(CalcTokenId::MULTIPLY, "*", 1), "2")).WillOnce(Return("MUL_2_2"));
  EXPECT_CALL(mock_parser_factory, CreateNonTermTermNonTerm(RuleId::kRule1, "MUL_2_2", MockToken(CalcTokenId::MULTIPLY, "*", 1), "3"))
      .WillOnce(Return("MUL_*_3"));
  EXPECT_CALL(mock_parser_factory, CreateNonTermTermNonTerm(RuleId::kRule1, "MUL_*_3", MockToken(CalcTokenId::DIVIDE, "/", 1), "4"))
      .WillOnce(Return("DIV_*_4"));
  EXPECT_CALL(mock_parser_factory, CreateNull()).WillRepeatedly(Return("NULL"));
  auto result = parser.Expr(input.begin(), input.end());
  EXPECT_EQ(result.node, "DIV_*_4");
}

TEST(ParserTest, OneInteger_With_Missing_Closing_Brace) {
  vector<MockToken> input;
  input.push_back(MockToken(CalcTokenId::LPARENS, "(", 1));
  input.push_back(MockToken(CalcTokenId::INTEGER, "3", 1));

  MockParserFactory<NonTermType> mock_parser_factory;
  Parser<MockGrammar> parser(mock_parser_factory);
  EXPECT_CALL(mock_parser_factory, CreateNull()).WillRepeatedly(Return("NULL"));
  EXPECT_CALL(mock_parser_factory, CreateTerm).WillOnce(Return("Num"));
  auto result = parser.Expr(input.begin(), input.end());

  EXPECT_EQ(result.node, "NULL");
  EXPECT_EQ(result.error_msg, "Error: Rule#0 doesnt't match");
}

TEST(ParserTest, WrongFormat) {
  vector<MockToken> input;
  input.push_back(MockToken(CalcTokenId::INTEGER, "1", 1));
  input.push_back(MockToken(CalcTokenId::INTEGER, "3", 1));

  MockParserFactory<NonTermType> mock_parser_factory;
  Parser<MockGrammar> parser(mock_parser_factory);
  EXPECT_CALL(mock_parser_factory, CreateNull()).WillRepeatedly(Return("NULL"));
  EXPECT_CALL(mock_parser_factory, CreateTerm).WillOnce(Return("Num"));
  auto result = parser.Expr(input.begin(), input.end());
  EXPECT_EQ(result.node, "NULL");
  EXPECT_EQ(result.error_msg, "ERROR: Tokens left");
}

TEST(ParserTest, WrongFormat2) {
  vector<MockToken> input;
  input.push_back(MockToken(CalcTokenId::INTEGER, "1", 1));
  input.push_back(MockToken(CalcTokenId::PLUS, "+", 1));

  MockParserFactory<NonTermType> mock_parser_factory;
  Parser<MockGrammar> parser(mock_parser_factory);
  EXPECT_CALL(mock_parser_factory, CreateNull()).WillRepeatedly(Return("NULL"));
  EXPECT_CALL(mock_parser_factory, CreateTerm).WillOnce(Return("Num"));
  auto result = parser.Expr(input.begin(), input.end());
  EXPECT_EQ(result.node, "NULL");
  EXPECT_EQ(result.error_msg, "ERROR: Tokens left");
}
