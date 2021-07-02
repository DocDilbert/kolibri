#include "lexer/lexer_rules.h"

#include <gtest/gtest.h>

#include <iostream>
#include <sstream>
#include <string>
#include <string_view>

using namespace lexer;
using namespace std;


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
  static constexpr const char* GetString() { return "Test1"; }
};

class MatcherStringTest : public ::testing::Test {
 protected:
  void SetUp() override {}
};

TEST_F(MatcherStringTest, stringMatch) {
  MatcherString<MockStringProviderTestStr1> matcher_str;
  const char* test_str = "Test1____";
  auto* result_ptr = matcher_str.Parse(test_str, &test_str[strlen(test_str)]);
  EXPECT_EQ(&test_str[5], result_ptr);
}

TEST_F(MatcherStringTest, stringMatchInputTooShort) {
  MatcherString<MockStringProviderTestStr1> matcher_str;
  const char* test_str = "Tes";
  auto* result_ptr = matcher_str.Parse(test_str, &test_str[strlen(test_str)]);
  EXPECT_EQ(&test_str[0], result_ptr);
}

TEST_F(MatcherStringTest, stringNoMatch) {
  MatcherString<MockStringProviderTestStr1> matcher_str;
  const char* test_str = "TEst1____";
  auto* result_ptr = matcher_str.Parse(test_str, &test_str[strlen(test_str)]);
  EXPECT_EQ(&test_str[0], result_ptr);
}

TEST_F(MatcherStringTest, stringMatchCaseSensitiveNoMatch) {
  MatcherString<MockStringProviderTestStr1> matcher_str;
  const char* test_str = "teSt1___";
  auto* result_ptr = matcher_str.Parse(test_str, &test_str[strlen(test_str)]);
  EXPECT_EQ(&test_str[0], result_ptr);
}

TEST_F(MatcherStringTest, stringMatchCaseInsensitiveMatch) {
  MatcherString<MockStringProviderTestStr1, true> matcher_str;
  const char* test_str = "teSt1____";
  auto* result_ptr = matcher_str.Parse(test_str, &test_str[strlen(test_str)]);
  EXPECT_EQ(&test_str[5], result_ptr);
}
//----------------------------------------------------------------------------
// MatcherRangeByStartStopDelimiter Tests
//----------------------------------------------------------------------------
class MatcherRangeByStartStopDelimiterTest : public ::testing::Test {
 protected:
  void SetUp() override {}
};

template <char ch>
class MockMatcherCh {
 public:
  const char* Parse(const char* begin, const char* end) {
    if (*begin == ch) {
      return begin + 1;
    }
    return begin;
  }
};

TEST_F(MatcherRangeByStartStopDelimiterTest, matchStartStopWithSpan0ExpectedEmptyToken) {
  MatcherRangeByStartStopDelimiter< MockMatcherCh<'A'>, MockMatcherCh<'B'>> matcher_sequence;
  const char* test_str = "ABC";
  auto* result_ptr = matcher_sequence.Parse(test_str, &test_str[strlen(test_str)]);
 
  EXPECT_EQ(&test_str[2], result_ptr);
}

TEST_F(MatcherRangeByStartStopDelimiterTest, matchStartStopWithSpan1ExpectedEmptyToken) {
  MatcherRangeByStartStopDelimiter< MockMatcherCh<'A'>, MockMatcherCh<'B'>> matcher_sequence;
  const char* test_str = "A_BC";
  auto* result_ptr = matcher_sequence.Parse(test_str, &test_str[strlen(test_str)]);
 
  EXPECT_EQ(&test_str[3], result_ptr);
}

TEST_F(MatcherRangeByStartStopDelimiterTest, matchStartStopWithSpan3ExpectedEmptyToken) {
  MatcherRangeByStartStopDelimiter< MockMatcherCh<'A'>, MockMatcherCh<'B'>> matcher_sequence;
  const char* test_str = "A___BC";
  auto* result_ptr = matcher_sequence.Parse(test_str, &test_str[strlen(test_str)]);
 
  EXPECT_EQ(&test_str[5], result_ptr);
}

TEST_F(MatcherRangeByStartStopDelimiterTest, matchStartStopWithSpan0NoStartMatch) {
  MatcherRangeByStartStopDelimiter< MockMatcherCh<'A'>, MockMatcherCh<'B'>> matcher_sequence;
  const char* test_str = "BC";
  auto* result_ptr = matcher_sequence.Parse(test_str, &test_str[strlen(test_str)]);
 
  EXPECT_EQ(&test_str[0], result_ptr);
}

TEST_F(MatcherRangeByStartStopDelimiterTest, matchStartStopWithSpan0NoStopMatch) {
  MatcherRangeByStartStopDelimiter< MockMatcherCh<'A'>, MockMatcherCh<'B'>> matcher_sequence;
  const char* test_str = "A_C";
  auto* result_ptr = matcher_sequence.Parse(test_str, &test_str[strlen(test_str)]);
 
  EXPECT_EQ(&test_str[0], result_ptr);
}

//----------------------------------------------------------------------------
// MatcherSequence Test
//----------------------------------------------------------------------------
class MatcherSequenceTest : public ::testing::Test {
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

TEST_F(MatcherSequenceTest, oneMatcherWithMatch) {
  MatcherSequence<MockMatcher<'A'>> matcher_sequence;
  const char* test_str = "AB";
  auto* result_ptr = matcher_sequence.Parse(test_str, &test_str[strlen(test_str)]);
  EXPECT_EQ(&test_str[1], result_ptr);
}

TEST_F(MatcherSequenceTest, twoMatchersWithMatch) {
  MatcherSequence<MockMatcher<'A'>, MockMatcher<'B'>> matcher_sequence;
  const char* test_str = "AB";
  auto* result_ptr = matcher_sequence.Parse(test_str, &test_str[strlen(test_str)]);
  EXPECT_EQ(&test_str[2], result_ptr);
}

TEST_F(MatcherSequenceTest, twoMatchersInputTooShort) {
  MatcherSequence<MockMatcher<'A'>, MockMatcher<'B'>> matcher_sequence;
  const char* test_str = "AB";                                                   // intentional no null termination
  auto* result_ptr = matcher_sequence.Parse(test_str, &test_str[strlen(test_str) - 1]);  // string stops at A
  EXPECT_EQ(&test_str[0], result_ptr);
}

TEST_F(MatcherSequenceTest, twoMatchersWithFirstNoMatch) {
  MatcherSequence<MockMatcher<'_'>, MockMatcher<'B'>> matcher_sequence;
  const char* test_str = "AB";
  auto* result_ptr = matcher_sequence.Parse(test_str, &test_str[strlen(test_str)]);
  EXPECT_EQ(&test_str[0], result_ptr);
}

TEST_F(MatcherSequenceTest, twoMatchersWithSecondNoMatch) {
  MatcherSequence<MockMatcher<'A'>, MockMatcher<'_'>> matcher_sequence;
  const char* test_str = "AB";
  auto* result_ptr = matcher_sequence.Parse(test_str, &test_str[strlen(test_str)]);
  EXPECT_EQ(&test_str[0], result_ptr);
}

//----------------------------------------------------------------------------
// LexerRules Tests
//----------------------------------------------------------------------------
class LexerRulesTest : public ::testing::Test {
 protected:
  void SetUp() override {}
};

using MockToken = string;

template <unsigned Tid>
class MockLexerRulesFactory {
 public:
  using value_type = MockToken;

  value_type Create(const char* begin, const char* end) { return "MockFactory" + to_string(Tid); }
};

template <unsigned Tid, char TMatchChar, typename TProduction = MockLexerRulesFactory<Tid>>
class MockLexerRulesMatcher {
 public:
  using production_type = TProduction;

  const char* Match(const char* begin, const char* end) {
    if (*begin == TMatchChar) {
      return begin + 1;
    }
    return begin;
  }

  MockToken Create(const char* begin, const char* end) {
    return "Create" + to_string(Tid);
    ;
  }
};

TEST_F(LexerRulesTest, NoMatch) {
  LexerRules<                    //
      MockLexerRulesFactory<0>,  // unknown factory
      MockLexerRulesFactory<1>,  // end of file factory
      MockLexerRulesMatcher<0, '_'>>
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
      MockLexerRulesMatcher<0, '_'>>
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
      MockLexerRulesMatcher<0, 'T'>>
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
      MockLexerRulesMatcher<0, '_'>,  //
      MockLexerRulesMatcher<1, 'T'>>
      rules;
  const char* test_str = "Test1____";
  auto match_result = rules.Match(test_str, &test_str[strlen(test_str)]);
  EXPECT_EQ(match_result, "Create1");
  EXPECT_EQ(rules.GetPosition(), &test_str[1]);
}

TEST_F(LexerRulesTest, BothRulesMatchFirstIsSkipped) {
  LexerRules<                                       //
      MockLexerRulesFactory<0>,                     // unknown factory
      MockLexerRulesFactory<1>,                     // end of file factory
      MockLexerRulesMatcher<0, 'T', SkipProduction>,  //
      MockLexerRulesMatcher<1, 'e'>>
      rules;
  const char* test_str = "Test1____";
  auto match_result = rules.Match(test_str, &test_str[strlen(test_str)]);
  EXPECT_EQ(match_result, "Create1");
  EXPECT_EQ(rules.GetPosition(), &test_str[2]);
}
