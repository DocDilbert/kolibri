#include "base/token.h"

#include <gtest/gtest.h>

#include <iostream>
#include <sstream>
#include <string>

using namespace base;
using namespace std;

enum MockTokenId {
  kOneTok,
  kTwoTok,
};

struct MockTokenIdConverter {
  static constexpr const char* ToString(MockTokenId id) {
    switch (id) {
      case kOneTok:
        return "kOneTok";
      case kTwoTok:
        return "kTwoTok";
      default:
        return "default";
    }
  }
};

using MockToken = Token<MockTokenId, MockTokenIdConverter>;

TEST(TokenTest, CompareTokensConstructCharArrayWithSizeShouldBeEqual) {
  stringstream ss;
  const char* number = "321";
  MockToken token1(kOneTok, number, strlen(number));
  MockToken token2(kOneTok, number, strlen(number));

  EXPECT_EQ(token1, token2);
}

TEST(TokenTest, CompareTokensConstructWithoutCharArrayShouldBeEqual) {
  MockToken token1(kTwoTok);
  MockToken token2(kTwoTok);

  EXPECT_EQ(token1, token2);
}

TEST(TokenTest, CompareTokensShouldBeNotEqual) {
  MockToken token1(kOneTok);
  MockToken token2(kTwoTok);

  EXPECT_NE(token1, token2);
}

TEST(TokenTest, ConstructTokenArrayWithSizeShouldBeConstructedCorretly) {
  const char* number = "321";
  MockToken token(kOneTok, number, strlen(number));

  EXPECT_EQ(token.GetId(), kOneTok);
  EXPECT_EQ(token.GetValue(), "321");
  EXPECT_EQ(token.GetStringId(), "kOneTok");
}

TEST(TokenTest, ConstructTokenArrayWithEndPtrShouldBeConstructedCorretly) {
  const char* number = "321";
  MockToken token(kOneTok, number, &number[3]);

  EXPECT_EQ(token.GetId(), kOneTok);
  EXPECT_EQ(token.GetValue(), "321");
  EXPECT_EQ(token.GetStringId(), "kOneTok");
}

TEST(TokenTest, ConstructTokenWithoutValueShouldBeConstructedCorrectly) {
  stringstream ss;
  MockToken token(kTwoTok);

  EXPECT_EQ(token.GetId(), kTwoTok);
  EXPECT_EQ(token.GetValue(), "");
  EXPECT_EQ(token.GetStringId(), "kTwoTok");
}

TEST(TokenTest, CopyConstructorCharArrayWithSizeShouldBeCopiedCorrectly) {
  const char* number = "321";
  MockToken token_orig(kOneTok, number, strlen(number));
  MockToken token(token_orig);

  EXPECT_EQ(token.GetId(), kOneTok);
  EXPECT_EQ(token.GetValue(), "321");
  EXPECT_EQ(token.GetStringId(), "kOneTok");
}

TEST(TokenTest, CopyAssignmentCharArrayWithSizeShouldBeAssignedCorrectly) {
  const char* number = "321";
  MockToken token_orig(kOneTok, number, strlen(number));
  MockToken token(kTwoTok);
  token = token_orig;

  EXPECT_EQ(token.GetId(), kOneTok);
  EXPECT_EQ(token.GetValue(), "321");
  EXPECT_EQ(token.GetStringId(), "kOneTok");
}
