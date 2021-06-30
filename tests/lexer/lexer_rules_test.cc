#include "lexer/lexer_rules.h"

#include <gtest/gtest.h>

#include <iostream>
#include <sstream>
#include <string>
#include <string_view>

using namespace lexer;
using namespace std;

class MockTokenFactory {
 public:
  using value_type = string;
  std::string Create(const char* begin, const char* end) {
    stringstream ss;
    ss << "MockTokenFactory: " << string_view(begin, std::distance(begin, end));
    return ss.str();
  }
};

//----------------------------------------------------------------------------
// MatcherPredicate Tests
//----------------------------------------------------------------------------
class MatcherPredicateTest : public ::testing::Test {
 protected:
  void SetUp() override {}
};

template <bool Return>
struct MockPredicate {
  bool operator()(char ch) { return (Return); }
};

TEST_F(MatcherPredicateTest, PredicateTrue) {
  MatcherPredicate<MockPredicate<true>> match_predicate;
  const char* test_str = "AB";
  auto* result_ptr = match_predicate.Parse(test_str, &test_str[strlen(test_str)]);
  EXPECT_EQ(&test_str[1], result_ptr);
}

TEST_F(MatcherPredicateTest, PredicateFalse) {
  MatcherPredicate<MockPredicate<false>> match_predicate;
  const char* test_str = "aB";
  auto* result_ptr = match_predicate.Parse(test_str, &test_str[strlen(test_str)]);
  EXPECT_EQ(&test_str[0], result_ptr);
}

//----------------------------------------------------------------------------
// MatcherString Tests
//----------------------------------------------------------------------------
struct MockStringProviderTestStr1 {
  constexpr const char* GetString() { return "Test1"; }
};

class MatcherStringTest : public ::testing::Test {
 protected:
  void SetUp() override {}
};

TEST_F(MatcherStringTest, stringMatch) {
  MatcherString<MockStringProviderTestStr1> match_string;
  const char* test_str = "Test1____";
  auto* result_ptr = match_string.Parse(test_str, &test_str[strlen(test_str)]);
  EXPECT_EQ(&test_str[5], result_ptr);
}

TEST_F(MatcherStringTest, stringMatchInputTooShort) {
  MatcherString<MockStringProviderTestStr1> match_string;
  const char* test_str = "Tes";
  auto* result_ptr = match_string.Parse(test_str, &test_str[strlen(test_str)]);
  EXPECT_EQ(&test_str[0], result_ptr);
}

TEST_F(MatcherStringTest, stringNoMatch) {
  MatcherString<MockStringProviderTestStr1> match_string;
  const char* test_str = "TEst1____";
  auto* result_ptr = match_string.Parse(test_str, &test_str[strlen(test_str)]);
  EXPECT_EQ(&test_str[0], result_ptr);
}

TEST_F(MatcherStringTest, stringMatchCaseSensitiveNoMatch) {
  MatcherString<MockStringProviderTestStr1> match_string;
  const char* test_str = "teSt1___";
  auto* result_ptr = match_string.Parse(test_str, &test_str[strlen(test_str)]);
  EXPECT_EQ(&test_str[0], result_ptr);
}

TEST_F(MatcherStringTest, stringMatchCaseInsensitiveMatch) {
  MatcherString<MockStringProviderTestStr1, true> match_string;
  const char* test_str = "teSt1____";
  auto* result_ptr = match_string.Parse(test_str, &test_str[strlen(test_str)]);
  EXPECT_EQ(&test_str[5], result_ptr);
}

//----------------------------------------------------------------------------
// SequenceRule Tests
//----------------------------------------------------------------------------
class SequenceRuleTest : public ::testing::Test {
 protected:
  void SetUp() override {}
};

template <char ch>
class MockMatcher {
 public:
  const char* Parse(const char* begin, const char* end) {
    if (*begin == ch) {
      return begin + 1;
    }
    return begin;
  }
};

TEST_F(SequenceRuleTest, oneMatcherWithMatch) {
  SequenceRule<MockTokenFactory, MockMatcher<'A'>> seq_rule;
  const char* test_str = "AB";
  auto* result_ptr = seq_rule.Match(test_str, &test_str[strlen(test_str)]);
  auto result_create = seq_rule.Create(test_str, result_ptr);
  EXPECT_EQ(&test_str[1], result_ptr);
  EXPECT_EQ("MockTokenFactory: A", result_create);
}

TEST_F(SequenceRuleTest, twoMatchersWithMatch) {
  SequenceRule<MockTokenFactory, MockMatcher<'A'>, MockMatcher<'B'>> seq_rule;
  const char* test_str = "AB";
  auto* result_ptr = seq_rule.Match(test_str, &test_str[strlen(test_str)]);
  auto result_create = seq_rule.Create(test_str, result_ptr);
  EXPECT_EQ(&test_str[2], result_ptr);
  EXPECT_EQ("MockTokenFactory: AB", result_create);
}

TEST_F(SequenceRuleTest, twoMatchersInputTooShort) {
  SequenceRule<MockTokenFactory, MockMatcher<'A'>, MockMatcher<'B'>> seq_rule;
  const char* test_str = "AB";                                                   // intentional no null termination
  auto* result_ptr = seq_rule.Match(test_str, &test_str[strlen(test_str) - 1]);  // string stops at A
  auto result_create = seq_rule.Create(test_str, result_ptr);
  EXPECT_EQ(&test_str[0], result_ptr);
  EXPECT_EQ("MockTokenFactory: ", result_create);
}

TEST_F(SequenceRuleTest, twoMatchersWithFirstNoMatch) {
  SequenceRule<MockTokenFactory, MockMatcher<'_'>, MockMatcher<'B'>> seq_rule;
  const char* test_str = "AB";
  auto* result_ptr = seq_rule.Match(test_str, &test_str[strlen(test_str)]);
  auto result_create = seq_rule.Create(test_str, result_ptr);
  EXPECT_EQ(&test_str[0], result_ptr);
  EXPECT_EQ("MockTokenFactory: ", result_create);
}

TEST_F(SequenceRuleTest, twoMatchersWithSecondNoMatch) {
  SequenceRule<MockTokenFactory, MockMatcher<'A'>, MockMatcher<'_'>> seq_rule;
  const char* test_str = "AB";
  auto* result_ptr = seq_rule.Match(test_str, &test_str[strlen(test_str)]);
  auto result_create = seq_rule.Create(test_str, result_ptr);
  EXPECT_EQ(&test_str[0], result_ptr);
  EXPECT_EQ("MockTokenFactory: ", result_create);
}

//----------------------------------------------------------------------------
// LexerRules Tests
//----------------------------------------------------------------------------
class LexerRulesTest : public ::testing::Test {
 protected:
  void SetUp() override {}
};

using MockToken = string;

template <unsigned Tid, bool TMatch>
class MockLexerRulesMatcher {
 public:
  const char* Match(const char* begin, const char* end) {
    if (TMatch == true) {
      return begin + 1;
    }
    return begin;
  }

  MockToken Create(const char* begin, const char* end) {
    return "Create" + to_string(Tid);
    ;
  }
};

template <unsigned Tid>
class MockLexerRulesFactory {
 public:
  using value_type = MockToken;

  value_type Create(const char* begin, const char* end) { return "MockFactory" + to_string(Tid); }
};

TEST_F(LexerRulesTest, NoMatch) {
  LexerRules<                    //
      MockLexerRulesFactory<0>,  // unknown factory
      MockLexerRulesFactory<1>,  // end of file factory
      MockLexerRulesMatcher<0, false>>
      rules;
  const char* test_str = "Test1____";
  auto match_result = rules.Match(test_str, &test_str[strlen(test_str)]);
  EXPECT_EQ(match_result, "MockFactory0");
  EXPECT_EQ(rules.GetPosition(), &test_str[1]);  // Unknown char gets consumed
}

TEST_F(LexerRulesTest, EndOfFileButMatch) {
  LexerRules<                    //
      MockLexerRulesFactory<0>,  // unknown factory
      MockLexerRulesFactory<1>,  // end of file factory
      MockLexerRulesMatcher<0, true>>
      rules;
  const char* test_str = "";
  auto match_result = rules.Match(test_str, &test_str[strlen(test_str)]);
  EXPECT_EQ(match_result, "MockFactory1");
  EXPECT_EQ(rules.GetPosition(), &test_str[0]);  // Unknown char gets consumed
}

TEST_F(LexerRulesTest, FirstRuleMatch) {
  LexerRules<                    //
      MockLexerRulesFactory<0>,  // unknown factory
      MockLexerRulesFactory<1>,  // end of file factory
      MockLexerRulesMatcher<0, true>>
      rules;
  const char* test_str = "Test1____";
  auto match_result = rules.Match(test_str, &test_str[strlen(test_str)]);
  EXPECT_EQ(match_result, "Create0");
  EXPECT_EQ(rules.GetPosition(), &test_str[1]);
}

TEST_F(LexerRulesTest, SecondRuleMatch) {
  LexerRules<                           //
      MockLexerRulesFactory<0>,         // unknown factory
      MockLexerRulesFactory<1>,         // end of file factory
      MockLexerRulesMatcher<0, false>,  //
      MockLexerRulesMatcher<1, true>>
      rules;
  const char* test_str = "Test1____";
  auto match_result = rules.Match(test_str, &test_str[strlen(test_str)]);
  EXPECT_EQ(match_result, "Create1");
  EXPECT_EQ(rules.GetPosition(), &test_str[1]);
}
