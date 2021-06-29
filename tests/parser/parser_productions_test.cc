#include "parser/parser_productions.h"

#include <gmock/gmock.h>  // Brings in Google Mock.
#include <gtest/gtest.h>

#include <iostream>
#include <sstream>
#include <string>
#include <string_view>

using namespace parser;
using namespace std;

using ::testing::_;
using ::testing::DoAll;
using ::testing::Ref;
using ::testing::Return;
using ::testing::SetArgReferee;

class MockAstTraits {
 public:
  using value_type = string;
  using reference_type = string;
  using pointer_type = string;
};

using NonTermType=std::string;

struct MockToken {
  std::string value;
};

class MockParserFactory : public IParserFactory<NonTermType, MockToken> {
 public:
  using nonterm_type = NonTermType;
  using term_type = MockToken;

  MOCK_METHOD0(CreateNull, nonterm_type());
  MOCK_METHOD1(CreateEmpty, nonterm_type(parser::RuleId rule_id));
  MOCK_METHOD2(CreateTerm, nonterm_type(parser::RuleId rule_id, term_type term));
  MOCK_METHOD2(CreateNonTerm, nonterm_type(parser::RuleId rule_id, nonterm_type nonterm));

  MOCK_METHOD2(CreateNonTermList, nonterm_type(parser::RuleId rule_id, std::vector<nonterm_type> statements));
  MOCK_METHOD3(CreateTermNonTermList, nonterm_type(parser::RuleId rule_id, std::vector<term_type> terms, std::vector<nonterm_type> nonterms ));
  
  MOCK_METHOD3(CreateTermNonTerm, nonterm_type(parser::RuleId rule_id, term_type term, nonterm_type nonterm));
  MOCK_METHOD3(CreateNonTermNonTerm, nonterm_type(parser::RuleId rule_id, nonterm_type lhs, nonterm_type rhs));
  MOCK_METHOD4(CreateNonTermTermNonTerm, nonterm_type(parser::RuleId rule_id, nonterm_type lhs, term_type term, nonterm_type rhs));
};

//----------------------------------------------------------------------------
// EmptyProduction Test
//----------------------------------------------------------------------------
class EmptyProductionTest : public ::testing::Test {
 protected:
  void SetUp() override {}

 public:
  MockParserFactory mock_parser_factory_;
};

TEST_F(EmptyProductionTest, Create) {
  EmptyProduction<NonTermType, MockToken> nop_production(RuleId::kRule0);
  EXPECT_CALL(mock_parser_factory_, CreateNull).Times(0);
  EXPECT_CALL(mock_parser_factory_, CreateEmpty).Times(1);
  EXPECT_CALL(mock_parser_factory_, CreateTerm).Times(0);
  EXPECT_CALL(mock_parser_factory_, CreateTermNonTerm).Times(0);
  EXPECT_CALL(mock_parser_factory_, CreateNonTermTermNonTerm).Times(0);

  auto res = nop_production.Create(mock_parser_factory_);
  EXPECT_EQ(res, "");
}