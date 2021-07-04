#include "languages/calc/calc_interpreter.h"

#include <gmock/gmock.h>  // Brings in Google Mock.
#include <gtest/gtest.h>

#include <string>

using namespace languages::calc;
using namespace std;
using namespace languages;

using ::testing::_;
using ::testing::Ref;
using ::testing::Return;

enum class MockTokenId { kNone, kPlus, kMinus, kMultiply, kDiv };

struct MockToken {
  using id_type = MockTokenId;

  MockToken() : id_(MockTokenId::kNone), value_("None") {}
  MockToken(std::string value) : id_(MockTokenId::kNone), value_(value) {}
  MockToken(MockTokenId id) : id_(id), value_("None") {}

  std::string GetValue() { return value_; }
  MockTokenId GetId() { return id_; }

 private:
  MockTokenId id_;
  std::string value_;
};

template <typename T>
class MockMakePtr {
 public:
  using type = T*;
};

class MockAstConst : public AstConst<MockMakePtr, MockToken> {
 public:
  explicit MockAstConst(ConstType const_type, term_type value) : AstConst<MockMakePtr, MockToken>(const_type, value) {}
  MOCK_METHOD1(Accept, void(IAstVisitor<MockMakePtr, MockToken>& value));
};

class MockAstUnaryOp : public AstUnaryOp<MockMakePtr, MockToken> {
 public:
  explicit MockAstUnaryOp(MockTokenId oper, nonterm_type operand) : AstUnaryOp<MockMakePtr, MockToken>(MockToken(oper), operand) {}
  MOCK_METHOD1(Accept, void(IAstVisitor<MockMakePtr, MockToken>& value));
};

class MockAstBinaryOp : public AstBinaryOp<MockMakePtr, MockToken> {
 public:
  explicit MockAstBinaryOp(MockTokenId oper, nonterm_type operand_lhs, nonterm_type operand_rhs)
      : AstBinaryOp<MockMakePtr, MockToken>(operand_lhs, MockToken(oper), operand_rhs) {}
  MOCK_METHOD1(Accept, void(IAstVisitor<MockMakePtr, MockToken>& value));
};

TEST(CalcInterpreterTest, Visit_Num) {
  MockAstConst ast_const(ConstType::kInteger, MockToken("3"));

  EXPECT_CALL(ast_const, Accept(_)).WillOnce([&](IAstVisitor<MockMakePtr, MockToken>& value) { value.Visit(ast_const); });

  CalcInterpreter<MockMakePtr, MockToken> CalcInterpreter;
  auto res = CalcInterpreter.Interpret(&ast_const);
  EXPECT_EQ("3", res.expr);
}

TEST(CalcInterpreterTest, Visit_Unary_Num) {
  MockAstConst ast_const(ConstType::kInteger, MockToken("3"));
  EXPECT_CALL(ast_const, Accept(_)).WillOnce([&](IAstVisitor<MockMakePtr, MockToken>& value) { value.Visit(ast_const); });

  MockAstUnaryOp ast_unary(MockTokenId::kMinus, &ast_const);
  EXPECT_CALL(ast_unary, Accept(_)).WillOnce([&](IAstVisitor<MockMakePtr, MockToken>& value) {
    value.Visit(ast_unary);  //
  });

  CalcInterpreter<MockMakePtr, MockToken> CalcInterpreter;
  auto res = CalcInterpreter.Interpret(&ast_unary);
  EXPECT_EQ("-3", res.expr);
}

TEST(CalcInterpreterTest, Visit_BinOp_Add_2Factors) {
  MockAstConst ast_const3(ConstType::kInteger, MockToken("3"));
  EXPECT_CALL(ast_const3, Accept(_)).WillOnce([&](IAstVisitor<MockMakePtr, MockToken>& value) {
    value.Visit(ast_const3);  //
  });

  MockAstConst ast_const5(ConstType::kInteger, MockToken("5"));
  EXPECT_CALL(ast_const5, Accept(_)).WillOnce([&](IAstVisitor<MockMakePtr, MockToken>& value) {
    value.Visit(ast_const5);  //
  });

  MockAstBinaryOp ast_bin_op(MockTokenId::kPlus, &ast_const3, &ast_const5);
  EXPECT_CALL(ast_bin_op, Accept(_)).WillOnce([&](IAstVisitor<MockMakePtr, MockToken>& value) {
    value.Visit(ast_bin_op);  //
  });

  CalcInterpreter<MockMakePtr, MockToken> CalcInterpreter;
  auto res = CalcInterpreter.Interpret(&ast_bin_op);
  EXPECT_EQ("8", res.expr);
}

TEST(CalcInterpreterTest, Visit_BinOp_Add_3Factors) {
  MockAstConst ast_const1(ConstType::kInteger, MockToken("1"));
  EXPECT_CALL(ast_const1, Accept(_)).WillOnce([&](IAstVisitor<MockMakePtr, MockToken>& value) {
    value.Visit(ast_const1);  //
  });

  MockAstConst ast_const2(ConstType::kInteger, MockToken("2"));
  EXPECT_CALL(ast_const2, Accept(_)).WillOnce([&](IAstVisitor<MockMakePtr, MockToken>& value) {
    value.Visit(ast_const2);  //
  });

  MockAstConst ast_const3(ConstType::kInteger, MockToken("3"));
  EXPECT_CALL(ast_const3, Accept(_)).WillOnce([&](IAstVisitor<MockMakePtr, MockToken>& value) {
    value.Visit(ast_const3);  //
  });

  MockAstBinaryOp ast_bin_op(MockTokenId::kPlus, &ast_const1, &ast_const2);
  EXPECT_CALL(ast_bin_op, Accept(_)).WillOnce([&](IAstVisitor<MockMakePtr, MockToken>& value) {
    value.Visit(ast_bin_op);  //
  });
  MockAstBinaryOp ast_bin_op_2(MockTokenId::kPlus, &ast_bin_op, &ast_const3);
  EXPECT_CALL(ast_bin_op_2, Accept(_)).WillOnce([&](IAstVisitor<MockMakePtr, MockToken>& value) {
    value.Visit(ast_bin_op_2);  //
  });

  CalcInterpreter<MockMakePtr, MockToken> CalcInterpreter;
  auto res = CalcInterpreter.Interpret(&ast_bin_op_2);
  EXPECT_EQ("6", res.expr);
}

TEST(CalcInterpreterTest, Visit_BinOp_Mul_2Factors) {
  MockAstConst ast_const3(ConstType::kInteger, MockToken("3"));
  EXPECT_CALL(ast_const3, Accept(_)).WillOnce([&](IAstVisitor<MockMakePtr, MockToken>& value) {
    value.Visit(ast_const3);  //
  });

  MockAstConst ast_const5(ConstType::kInteger, MockToken("5"));
  EXPECT_CALL(ast_const5, Accept(_)).WillOnce([&](IAstVisitor<MockMakePtr, MockToken>& value) {
    value.Visit(ast_const5);  //
  });

  MockAstBinaryOp ast_bin_op(MockTokenId::kMultiply, &ast_const3, &ast_const5);
  EXPECT_CALL(ast_bin_op, Accept(_)).WillOnce([&](IAstVisitor<MockMakePtr, MockToken>& value) {
    value.Visit(ast_bin_op);  //
  });

  CalcInterpreter<MockMakePtr, MockToken> CalcInterpreter;
  auto res = CalcInterpreter.Interpret(&ast_bin_op);
  EXPECT_EQ("15", res.expr);
}
