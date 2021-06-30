#include "parser/parser_rules.h"

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

using NonTermType = std::string;

using MockToken = string;
using MockIterator = vector<MockToken>::iterator;

class MockParserFactory : public IParserFactory<NonTermType, string> {
 public:
  using nonterm_type = NonTermType;
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

class MockProduction {
 public:
  using Token = std::string;
  using nonterm_type = NonTermType;
  MOCK_METHOD1(AddTerminal, void(Token terminal));
  MOCK_METHOD1(Create, nonterm_type(IParserFactory<nonterm_type, Token>& parser_factory));
};

class MockParserGrammar : public IParserGrammar<NonTermType, MockIterator> {
 public:
  MOCK_METHOD4(Match, parser::RuleResult<NonTermType>(IParserFactory<NonTermType, string>& factory, RuleId id, MockIterator& it, MockIterator end));
};

void ParserGrammarActionIncrementIterator(IParserFactory<NonTermType, string>& factory, RuleId id, MockIterator& it, MockIterator end) { ++it; };

//----------------------------------------------------------------------------
// class SequenceExpr Test
//----------------------------------------------------------------------------
class SequenceExprTest : public ::testing::Test {
 protected:
  void SetUp() override {}

 public:
  vector<string> test_data;
  MockParserFactory parser_factory;
  MockParserGrammar grammar;
};

template <bool MatchResult>
class MockTerminalExpr {
 public:
  template <typename Production, typename TNonTerm, typename Iterator>
  Result Match(Production& production, IParserFactory<TNonTerm, typename Iterator::value_type>& parser_factory,
               IParserGrammar<TNonTerm, Iterator>& parser_grammar, Iterator& it, Iterator end) {
    if (it == end) {
      return Result(false, false, "");
    }
    if (MatchResult) {
      production.AddTerminal(*it);
      ++it;
    }

    return Result(MatchResult, false, "");
  }
};

TEST_F(SequenceExprTest, OneTerminalMatch) {
  SequenceExpr<MockTerminalExpr<true>> sequence_expr;
  test_data.push_back("4");
  test_data.push_back("8");

  auto it = test_data.begin();

  EXPECT_CALL(parser_factory, CreateNull).Times(0);
  EXPECT_CALL(parser_factory, CreateEmpty).Times(0);
  EXPECT_CALL(parser_factory, CreateTerm).Times(0);
  EXPECT_CALL(parser_factory, CreateTermNonTerm).Times(0);
  EXPECT_CALL(parser_factory, CreateNonTermTermNonTerm).Times(0);

  MockProduction production;
  EXPECT_CALL(production, AddTerminal).Times(1);
  EXPECT_CALL(production, Create).Times(0);
  auto result = sequence_expr.Match(production, parser_factory, grammar, it, test_data.end());

  EXPECT_EQ(it, test_data.begin() + 1);

  EXPECT_EQ(result.is_match, true);
  EXPECT_EQ(result.is_error, false);
  EXPECT_EQ(result.msg, "");
}

TEST_F(SequenceExprTest, OneTerminalNoMatch) {
  SequenceExpr<MockTerminalExpr<false>> sequence_expr;
  test_data.push_back("4");
  test_data.push_back("8");

  auto it = test_data.begin();

  EXPECT_CALL(parser_factory, CreateNull).Times(0);
  EXPECT_CALL(parser_factory, CreateEmpty).Times(0);
  EXPECT_CALL(parser_factory, CreateTerm).Times(0);
  EXPECT_CALL(parser_factory, CreateTermNonTerm).Times(0);
  EXPECT_CALL(parser_factory, CreateNonTermTermNonTerm).Times(0);

  MockProduction production;
  EXPECT_CALL(production, AddTerminal).Times(0);
  EXPECT_CALL(production, Create).Times(0);
  auto result = sequence_expr.Match(production, parser_factory, grammar, it, test_data.end());

  EXPECT_EQ(it, test_data.begin());
  EXPECT_EQ(result.is_match, false);
  EXPECT_EQ(result.is_error, false);
  EXPECT_EQ(result.msg, "SequenceExpr  -  No match");
}

TEST_F(SequenceExprTest, TwoTerminalsMatch) {
  SequenceExpr<MockTerminalExpr<true>, MockTerminalExpr<true>> sequence_rule;
  test_data.push_back("4");
  test_data.push_back("8");

  auto it = test_data.begin();

  EXPECT_CALL(parser_factory, CreateNull).Times(0);
  EXPECT_CALL(parser_factory, CreateEmpty).Times(0);
  EXPECT_CALL(parser_factory, CreateTerm).Times(0);
  EXPECT_CALL(parser_factory, CreateTermNonTerm).Times(0);
  EXPECT_CALL(parser_factory, CreateNonTermTermNonTerm).Times(0);

  MockProduction production;
  EXPECT_CALL(production, AddTerminal).Times(2);
  EXPECT_CALL(production, Create).Times(0);
  auto result = sequence_rule.Match(production, parser_factory, grammar, it, test_data.end());

  EXPECT_EQ(it, test_data.begin() + 2);
  EXPECT_EQ(result.is_match, true);
  EXPECT_EQ(result.is_error, false);
  EXPECT_EQ(result.msg, "");
}

TEST_F(SequenceExprTest, ThreeTerminalsMatchSequenceTooSmall) {
  SequenceExpr<MockTerminalExpr<true>, MockTerminalExpr<true>, MockTerminalExpr<true>> sequence_rule;
  test_data.push_back("4");
  test_data.push_back("8");

  auto it = test_data.begin();

  EXPECT_CALL(parser_factory, CreateNull).Times(0);
  EXPECT_CALL(parser_factory, CreateEmpty).Times(0);
  EXPECT_CALL(parser_factory, CreateTerm).Times(0);
  EXPECT_CALL(parser_factory, CreateTermNonTerm).Times(0);
  EXPECT_CALL(parser_factory, CreateNonTermTermNonTerm).Times(0);

  MockProduction production;
  EXPECT_CALL(production, AddTerminal).Times(2);
  EXPECT_CALL(production, Create).Times(0);
  auto result = sequence_rule.Match(production, parser_factory, grammar, it, test_data.end());

  EXPECT_EQ(it, test_data.begin());
  EXPECT_EQ(result.is_match, false);
  EXPECT_EQ(result.is_error, false);
  EXPECT_EQ(result.msg, "SequenceExpr  -  No match");
}

TEST_F(SequenceExprTest, TwoTerminalsFirstMatchSecondNoMatch) {
  SequenceExpr<MockTerminalExpr<true>, MockTerminalExpr<false>> sequence_rule;
  test_data.push_back("4");
  test_data.push_back("8");

  auto it = test_data.begin();

  EXPECT_CALL(parser_factory, CreateNull).Times(0);
  EXPECT_CALL(parser_factory, CreateEmpty).Times(0);
  EXPECT_CALL(parser_factory, CreateTerm).Times(0);
  EXPECT_CALL(parser_factory, CreateTermNonTerm).Times(0);
  EXPECT_CALL(parser_factory, CreateNonTermTermNonTerm).Times(0);

  MockProduction production;
  EXPECT_CALL(production, AddTerminal).Times(1);
  EXPECT_CALL(production, Create).Times(0);
  auto result = sequence_rule.Match(production, parser_factory, grammar, it, test_data.end());

  EXPECT_EQ(it, test_data.begin());
  EXPECT_EQ(result.is_match, false);
  EXPECT_EQ(result.is_error, false);
  EXPECT_EQ(result.msg, "SequenceExpr  -  No match");
}

TEST_F(SequenceExprTest, TwoTerminalsFirstNoMatchSecondMatch) {
  SequenceExpr<MockTerminalExpr<false>, MockTerminalExpr<true>> sequence_rule;
  test_data.push_back("4");
  test_data.push_back("8");

  auto it = test_data.begin();

  EXPECT_CALL(parser_factory, CreateNull).Times(0);
  EXPECT_CALL(parser_factory, CreateEmpty).Times(0);
  EXPECT_CALL(parser_factory, CreateTerm).Times(0);
  EXPECT_CALL(parser_factory, CreateTermNonTerm).Times(0);
  EXPECT_CALL(parser_factory, CreateNonTermTermNonTerm).Times(0);

  MockProduction production;
  EXPECT_CALL(production, AddTerminal).Times(0);
  EXPECT_CALL(production, Create).Times(0);
  auto result = sequence_rule.Match(production, parser_factory, grammar, it, test_data.end());

  EXPECT_EQ(it, test_data.begin());
  EXPECT_EQ(result.is_match, false);
  EXPECT_EQ(result.is_error, false);
  EXPECT_EQ(result.msg, "SequenceExpr  -  No match");
}

//----------------------------------------------------------------------------
// NMatchesOrMoreExpr Test
//----------------------------------------------------------------------------
class NMatchesOrMoreExprTest : public ::testing::Test {
 protected:
  void SetUp() override {}

 public:
  vector<string> test_data;
  MockParserFactory parser_factory;
  MockParserGrammar grammar;
};

class MockExprNoMatch {
 public:
  template <typename Production, typename TraitType, typename Iterator>
  Result Match(Production& production, IParserFactory<TraitType, typename Iterator::value_type>& parser_factory,
               IParserGrammar<TraitType, Iterator>& parser_grammar, Iterator& it, Iterator end) {
    return Result(false, false, "");
  }
};

class MockExprMatchOnFirst {
 public:
  template <typename Production, typename TraitType, typename Iterator>
  Result Match(Production& production, IParserFactory<TraitType, typename Iterator::value_type>& parser_factory,
               IParserGrammar<TraitType, Iterator>& parser_grammar, Iterator& it, Iterator end) {
    auto token = *it;

    if (token == "4") {
      ++it;
      production.AddTerminal(token);
      return Result(true, false, "");
    }
    return Result(false, false, "");
  }
};

TEST_F(NMatchesOrMoreExprTest, NEq0NoMatches) {
  NMatchesOrMoreExpr<0, MockExprNoMatch> n_or_more_expr;
  test_data.push_back("4");
  test_data.push_back("8");

  auto it = test_data.begin();

  EXPECT_CALL(parser_factory, CreateNull).Times(0);
  EXPECT_CALL(parser_factory, CreateEmpty).Times(0);
  EXPECT_CALL(parser_factory, CreateTerm).Times(0);
  EXPECT_CALL(parser_factory, CreateTermNonTerm).Times(0);
  EXPECT_CALL(parser_factory, CreateNonTermTermNonTerm).Times(0);

  MockProduction production;
  EXPECT_CALL(production, AddTerminal).Times(0);
  EXPECT_CALL(production, Create).Times(0);
  auto result = n_or_more_expr.Match(production, parser_factory, grammar, it, test_data.end());

  EXPECT_EQ(it, test_data.begin());

  EXPECT_EQ(result.is_match, true);
  EXPECT_EQ(result.is_error, false);
  EXPECT_EQ(result.msg, "");
}

TEST_F(NMatchesOrMoreExprTest, NEq0FirstExprMatches) {
  NMatchesOrMoreExpr<0, MockExprMatchOnFirst> n_or_more_expr;
  test_data.push_back("4");
  test_data.push_back("8");

  auto it = test_data.begin();

  EXPECT_CALL(parser_factory, CreateNull).Times(0);
  EXPECT_CALL(parser_factory, CreateEmpty).Times(0);
  EXPECT_CALL(parser_factory, CreateTerm).Times(0);
  EXPECT_CALL(parser_factory, CreateTermNonTerm).Times(0);
  EXPECT_CALL(parser_factory, CreateNonTermTermNonTerm).Times(0);

  MockProduction production;
  EXPECT_CALL(production, AddTerminal).Times(1);
  EXPECT_CALL(production, Create).Times(0);
  auto result = n_or_more_expr.Match(production, parser_factory, grammar, it, test_data.end());

  EXPECT_EQ(it, test_data.begin() + 1);

  EXPECT_EQ(result.is_match, true);
  EXPECT_EQ(result.is_error, false);
  EXPECT_EQ(result.msg, "");
}

TEST_F(NMatchesOrMoreExprTest, NEq0FirstAndSecondExprMatches) {
  NMatchesOrMoreExpr<0, MockExprMatchOnFirst> n_or_more_expr;
  test_data.push_back("4");
  test_data.push_back("4");
  test_data.push_back("8");

  auto it = test_data.begin();

  EXPECT_CALL(parser_factory, CreateNull).Times(0);
  EXPECT_CALL(parser_factory, CreateEmpty).Times(0);
  EXPECT_CALL(parser_factory, CreateTerm).Times(0);
  EXPECT_CALL(parser_factory, CreateTermNonTerm).Times(0);
  EXPECT_CALL(parser_factory, CreateNonTermTermNonTerm).Times(0);

  MockProduction production;
  EXPECT_CALL(production, AddTerminal).Times(2);
  EXPECT_CALL(production, Create).Times(0);
  auto result = n_or_more_expr.Match(production, parser_factory, grammar, it, test_data.end());

  EXPECT_EQ(it, test_data.begin() + 2);

  EXPECT_EQ(result.is_match, true);
  EXPECT_EQ(result.is_error, false);
  EXPECT_EQ(result.msg, "");
}

TEST_F(NMatchesOrMoreExprTest, NEq1NoMatches) {
  NMatchesOrMoreExpr<1, MockExprNoMatch> n_or_more_expr;
  test_data.push_back("4");
  test_data.push_back("8");

  auto it = test_data.begin();

  EXPECT_CALL(parser_factory, CreateNull).Times(0);
  EXPECT_CALL(parser_factory, CreateEmpty).Times(0);
  EXPECT_CALL(parser_factory, CreateTerm).Times(0);
  EXPECT_CALL(parser_factory, CreateTermNonTerm).Times(0);
  EXPECT_CALL(parser_factory, CreateNonTermTermNonTerm).Times(0);

  MockProduction production;
  EXPECT_CALL(production, AddTerminal).Times(0);
  EXPECT_CALL(production, Create).Times(0);
  auto result = n_or_more_expr.Match(production, parser_factory, grammar, it, test_data.end());

  EXPECT_EQ(it, test_data.begin());

  EXPECT_EQ(result.is_match, false);
  EXPECT_EQ(result.is_error, false);
  EXPECT_EQ(result.msg, "NMatchesOrMoreExpr: No match");
}

TEST_F(NMatchesOrMoreExprTest, NEq1FirstExprMatches) {
  NMatchesOrMoreExpr<1, MockExprMatchOnFirst> n_or_more_expr;
  test_data.push_back("4");
  test_data.push_back("8");

  auto it = test_data.begin();

  EXPECT_CALL(parser_factory, CreateNull).Times(0);
  EXPECT_CALL(parser_factory, CreateEmpty).Times(0);
  EXPECT_CALL(parser_factory, CreateTerm).Times(0);
  EXPECT_CALL(parser_factory, CreateTermNonTerm).Times(0);
  EXPECT_CALL(parser_factory, CreateNonTermTermNonTerm).Times(0);

  MockProduction production;
  EXPECT_CALL(production, AddTerminal).Times(1);
  EXPECT_CALL(production, Create).Times(0);
  auto result = n_or_more_expr.Match(production, parser_factory, grammar, it, test_data.end());

  EXPECT_EQ(it, test_data.begin() + 1);

  EXPECT_EQ(result.is_match, true);
  EXPECT_EQ(result.is_error, false);
  EXPECT_EQ(result.msg, "");
}

TEST_F(NMatchesOrMoreExprTest, NEq1FirstAndSecondExprMatches) {
  NMatchesOrMoreExpr<1, MockExprMatchOnFirst> n_or_more_expr;
  test_data.push_back("4");
  test_data.push_back("4");
  test_data.push_back("8");

  auto it = test_data.begin();

  EXPECT_CALL(parser_factory, CreateNull).Times(0);
  EXPECT_CALL(parser_factory, CreateEmpty).Times(0);
  EXPECT_CALL(parser_factory, CreateTerm).Times(0);
  EXPECT_CALL(parser_factory, CreateTermNonTerm).Times(0);
  EXPECT_CALL(parser_factory, CreateNonTermTermNonTerm).Times(0);

  MockProduction production;
  EXPECT_CALL(production, AddTerminal).Times(2);
  EXPECT_CALL(production, Create).Times(0);
  auto result = n_or_more_expr.Match(production, parser_factory, grammar, it, test_data.end());

  EXPECT_EQ(it, test_data.begin() + 2);

  EXPECT_EQ(result.is_match, true);
  EXPECT_EQ(result.is_error, false);
  EXPECT_EQ(result.msg, "");
}

//----------------------------------------------------------------------------
// class OrderedChoiceExpr Test
//----------------------------------------------------------------------------
class OrderedChoiceExprTest : public ::testing::Test {
 protected:
  void SetUp() override {}

 public:
  vector<string> test_data;
  MockParserFactory parser_factory;
  MockParserGrammar grammar;
};

template <bool MatchReturn>
class MockChoiceExpr {
 public:
  template <typename Production, typename TraitType, typename Iterator>
  Result Match(Production& production, IParserFactory<TraitType, typename Iterator::value_type>& parser_factory,
               IParserGrammar<TraitType, Iterator>& parser_grammar, Iterator& it, Iterator end) {
    if (MatchReturn) {
      ++it;
    }
    return Result(MatchReturn, false, "");
  }
};

TEST_F(OrderedChoiceExprTest, TwoChoicesFirstMatches) {
  OrderedChoiceExpr<MockChoiceExpr<true>, MockChoiceExpr<true>> sequence_expr;
  test_data.push_back("4");
  test_data.push_back("8");

  auto it = test_data.begin();

  EXPECT_CALL(parser_factory, CreateNull).Times(0);
  EXPECT_CALL(parser_factory, CreateEmpty).Times(0);
  EXPECT_CALL(parser_factory, CreateTerm).Times(0);
  EXPECT_CALL(parser_factory, CreateTermNonTerm).Times(0);
  EXPECT_CALL(parser_factory, CreateNonTermTermNonTerm).Times(0);

  MockProduction production;
  EXPECT_CALL(production, AddTerminal).Times(0);
  EXPECT_CALL(production, Create).Times(0);
  auto result = sequence_expr.Match(production, parser_factory, grammar, it, test_data.end());

  EXPECT_EQ(it, test_data.begin() + 1);

  EXPECT_EQ(result.is_match, true);
  EXPECT_EQ(result.is_error, false);
  EXPECT_EQ(result.msg, "");
}

TEST_F(OrderedChoiceExprTest, TwoChoicesSecondMatches) {
  OrderedChoiceExpr<MockChoiceExpr<false>, MockChoiceExpr<true>> sequence_expr;
  test_data.push_back("4");
  test_data.push_back("8");

  auto it = test_data.begin();

  EXPECT_CALL(parser_factory, CreateNull).Times(0);
  EXPECT_CALL(parser_factory, CreateEmpty).Times(0);
  EXPECT_CALL(parser_factory, CreateTerm).Times(0);
  EXPECT_CALL(parser_factory, CreateTermNonTerm).Times(0);
  EXPECT_CALL(parser_factory, CreateNonTermTermNonTerm).Times(0);

  MockProduction production;
  EXPECT_CALL(production, AddTerminal).Times(0);
  EXPECT_CALL(production, Create).Times(0);
  auto result = sequence_expr.Match(production, parser_factory, grammar, it, test_data.end());

  EXPECT_EQ(it, test_data.begin() + 1);

  EXPECT_EQ(result.is_match, true);
  EXPECT_EQ(result.is_error, false);
  EXPECT_EQ(result.msg, "");
}

TEST_F(OrderedChoiceExprTest, ThreeChoicesThirdMatches) {
  OrderedChoiceExpr<MockChoiceExpr<false>, MockChoiceExpr<false>, MockChoiceExpr<true>> sequence_expr;
  test_data.push_back("4");
  test_data.push_back("8");

  auto it = test_data.begin();

  EXPECT_CALL(parser_factory, CreateNull).Times(0);
  EXPECT_CALL(parser_factory, CreateEmpty).Times(0);
  EXPECT_CALL(parser_factory, CreateTerm).Times(0);
  EXPECT_CALL(parser_factory, CreateTermNonTerm).Times(0);
  EXPECT_CALL(parser_factory, CreateNonTermTermNonTerm).Times(0);

  MockProduction production;
  EXPECT_CALL(production, AddTerminal).Times(0);
  EXPECT_CALL(production, Create).Times(0);
  auto result = sequence_expr.Match(production, parser_factory, grammar, it, test_data.end());

  EXPECT_EQ(it, test_data.begin() + 1);

  EXPECT_EQ(result.is_match, true);
  EXPECT_EQ(result.is_error, false);
  EXPECT_EQ(result.msg, "");
}

TEST_F(OrderedChoiceExprTest, TwoChoicesNoneMatches) {
  OrderedChoiceExpr<MockChoiceExpr<false>, MockChoiceExpr<false>> sequence_expr;
  test_data.push_back("4");
  test_data.push_back("8");

  auto it = test_data.begin();

  EXPECT_CALL(parser_factory, CreateNull).Times(0);
  EXPECT_CALL(parser_factory, CreateEmpty).Times(0);
  EXPECT_CALL(parser_factory, CreateTerm).Times(0);
  EXPECT_CALL(parser_factory, CreateTermNonTerm).Times(0);
  EXPECT_CALL(parser_factory, CreateNonTermTermNonTerm).Times(0);

  MockProduction production;
  EXPECT_CALL(production, AddTerminal).Times(0);
  EXPECT_CALL(production, Create).Times(0);
  auto result = sequence_expr.Match(production, parser_factory, grammar, it, test_data.end());

  EXPECT_EQ(it, test_data.begin());

  EXPECT_EQ(result.is_match, false);
  EXPECT_EQ(result.is_error, false);
  EXPECT_EQ(result.msg, "OrderedChoiceExpr: No match");
}

//----------------------------------------------------------------------------
//  OptionalExpr Test
//----------------------------------------------------------------------------
class OptionalExprTest : public ::testing::Test {
 protected:
  void SetUp() override {}

 public:
  vector<string> test_data;
  MockParserFactory parser_factory;
  MockParserGrammar grammar;
};

template <bool MatchReturn>
class MockOptionalExpr {
 public:
  template <typename Production, typename TraitType, typename Iterator>
  Result Match(Production& production, IParserFactory<TraitType, typename Iterator::value_type>& parser_factory,
               IParserGrammar<TraitType, Iterator>& parser_grammar, Iterator& it, Iterator end) {
    if (MatchReturn) {
      ++it;
    }
    return Result(MatchReturn, false, "");
  }
};

TEST_F(OptionalExprTest, NoMatch) {
  OptionalExpr<MockOptionalExpr<false>> optional_expr;
  test_data.push_back("4");
  test_data.push_back("8");

  auto it = test_data.begin();

  EXPECT_CALL(parser_factory, CreateNull).Times(0);
  EXPECT_CALL(parser_factory, CreateEmpty).Times(0);
  EXPECT_CALL(parser_factory, CreateTerm).Times(0);
  EXPECT_CALL(parser_factory, CreateTermNonTerm).Times(0);
  EXPECT_CALL(parser_factory, CreateNonTermTermNonTerm).Times(0);

  MockProduction production;
  EXPECT_CALL(production, AddTerminal).Times(0);
  EXPECT_CALL(production, Create).Times(0);
  auto result = optional_expr.Match(production, parser_factory, grammar, it, test_data.end());

  EXPECT_EQ(it, test_data.begin());

  EXPECT_EQ(result.is_match, true);
  EXPECT_EQ(result.is_error, false);
  EXPECT_EQ(result.msg, "");
}

TEST_F(OptionalExprTest, Match) {
  OptionalExpr<MockOptionalExpr<true>> optional_expr;
  test_data.push_back("4");
  test_data.push_back("8");

  auto it = test_data.begin();

  EXPECT_CALL(parser_factory, CreateNull).Times(0);
  EXPECT_CALL(parser_factory, CreateEmpty).Times(0);
  EXPECT_CALL(parser_factory, CreateTerm).Times(0);
  EXPECT_CALL(parser_factory, CreateTermNonTerm).Times(0);
  EXPECT_CALL(parser_factory, CreateNonTermTermNonTerm).Times(0);

  MockProduction production;
  EXPECT_CALL(production, AddTerminal).Times(0);
  EXPECT_CALL(production, Create).Times(0);
  auto result = optional_expr.Match(production, parser_factory, grammar, it, test_data.end());

  EXPECT_EQ(it, test_data.begin() + 1);

  EXPECT_EQ(result.is_match, true);
  EXPECT_EQ(result.is_error, false);
  EXPECT_EQ(result.msg, "");
}
