#include "base/token.h"

#include <gtest/gtest.h>

#include <iostream>
#include <sstream>
#include <string>
#include "token_out.h"


using namespace languages::calc;
using namespace base;
using namespace std;

using MockToken = Token<languages::calc::CalcTokenId, languages::calc::CalcTokenIdConverter>;

TEST(TokenTest, IntegerToken_CharArray_With_Size) {
  stringstream ss;
  const char* number = "321";
  MockToken token(CalcTokenId::INTEGER, number, strlen(number));
  ss << token;

  EXPECT_EQ(ss.str(), "Token(INTEGER, 321)");

  EXPECT_EQ(token.GetId(), CalcTokenId::INTEGER);
  EXPECT_EQ(token.GetValue(), "321");
  EXPECT_EQ(token.GetStringId(), "INTEGER");
}

TEST(TokenTest, Compare_IntegerToken_CharArray_With_Size) {
  stringstream ss;
  const char* number = "321";
  MockToken token1(CalcTokenId::INTEGER, number, strlen(number));
  MockToken token2(CalcTokenId::INTEGER, number, strlen(number));

  EXPECT_EQ(token1, token2);
}

TEST(TokenTest, Compare_PlusToken) {
  stringstream ss;
  MockToken token1(CalcTokenId::PLUS);
  MockToken token2(CalcTokenId::PLUS);

  EXPECT_EQ(token1, token2);
}

TEST(TokenTest, Compare_PlusToken_NotEqual) {
  stringstream ss;
  MockToken token1(CalcTokenId::PLUS);
  MockToken token2(CalcTokenId::MINUS);

  EXPECT_NE(token1, token2);
}

TEST(TokenTest, IntegerToken_CharArray_With_EndPtr) {
  stringstream ss;
  const char* number = "321";
  MockToken token(CalcTokenId::INTEGER, number, &number[3]);
  ss << token;

  EXPECT_EQ(ss.str(), "Token(INTEGER, 321)");
  EXPECT_EQ(token.GetId(), CalcTokenId::INTEGER);
  EXPECT_EQ(token.GetValue(), "321");
  EXPECT_EQ(token.GetStringId(), "INTEGER");
}

TEST(TokenTest, PlusToken_Without_Value) {
  stringstream ss;
  MockToken token(CalcTokenId::PLUS);
  ss << token;
  EXPECT_EQ(ss.str(), "Token(PLUS)");
  EXPECT_EQ(token.GetId(), CalcTokenId::PLUS);
  EXPECT_EQ(token.GetValue(), "");
  EXPECT_EQ(token.GetStringId(), "PLUS");
}

TEST(TokenTest, IntegerToken_CopyConstructor_CharArray_With_Size) {
  stringstream ss;
  const char* number = "321";
  MockToken token_orig(CalcTokenId::INTEGER, number, strlen(number));
  MockToken token(token_orig);
  ss << token;

  EXPECT_EQ(ss.str(), "Token(INTEGER, 321)");

  EXPECT_EQ(token.GetId(), CalcTokenId::INTEGER);
  EXPECT_EQ(token.GetValue(), "321");
  EXPECT_EQ(token.GetStringId(), "INTEGER");
}

TEST(TokenTest, IntegerToken_CopyAssignment_CharArray_With_Size) {
  stringstream ss;
  const char* number = "321";
  MockToken token_orig(CalcTokenId::INTEGER, number, strlen(number));
  MockToken token(CalcTokenId::PLUS);
  token = token_orig;
  ss << token;

  EXPECT_EQ(ss.str(), "Token(INTEGER, 321)");

  EXPECT_EQ(token.GetId(), CalcTokenId::INTEGER);
  EXPECT_EQ(token.GetValue(), "321");
  EXPECT_EQ(token.GetStringId(), "INTEGER");
}
