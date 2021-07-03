#include <gmock/gmock.h>  // Brings in Google Mock.
#include <gtest/gtest.h>

#include <string>

#include "languages/calc/calc_interpreter.h"

using namespace base;
using namespace languages::calc;
using namespace std;

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
class MockMakeSharedPtr {
 public:
  using type = std::shared_ptr<T>;
};

class MockType {
 public:
  using value_type = MockType;
  using reference_type = MockType&;
  using pointer_type = std::shared_ptr<MockType>;

  MOCK_METHOD1(Accept, void(IAstVisitor<MockMakeSharedPtr, MockToken>& value));
};

class MockAstConst : public AstConst<MockMakeSharedPtr, MockToken> {
 public:
  explicit MockAstConst(ConstType const_type, term_type value) : AstConst<MockMakeSharedPtr, MockToken>(const_type, value) {}
  MOCK_METHOD1(Accept, void(IAstVisitor<MockMakeSharedPtr, MockToken>& value));
};

class MockAstUnaryOp : public AstUnaryOp<MockMakeSharedPtr, MockToken> {
 public:
  explicit MockAstUnaryOp(MockTokenId oper, nonterm_type operand) : AstUnaryOp<MockMakeSharedPtr, MockToken>(MockToken(oper), operand) {}
  MOCK_METHOD1(Accept, void(IAstVisitor<MockMakeSharedPtr, MockToken>& value));
};

class MockAstBinaryOp : public AstBinaryOp<MockMakeSharedPtr, MockToken> {
 public:
  explicit MockAstBinaryOp(MockTokenId oper, nonterm_type operand_lhs, nonterm_type operand_rhs)
      : AstBinaryOp<MockMakeSharedPtr, MockToken>(operand_lhs, MockToken(oper), operand_rhs) {}
  MOCK_METHOD1(Accept, void(IAstVisitor<MockMakeSharedPtr, MockToken>& value));
};

TEST(CalcInterpreterTest, Visit_Num) {
  auto ast_const = std::make_shared<MockAstConst>(ConstType::kInteger, MockToken("3"));

  EXPECT_CALL(*ast_const, Accept(_)).WillOnce([&](IAstVisitor<MockMakeSharedPtr, MockToken>& value) { value.Visit(*ast_const); });

  CalcInterpreter<MockMakeSharedPtr, MockToken> CalcInterpreter;
  auto res = CalcInterpreter.Interpret(ast_const);
  EXPECT_EQ("3", res.expr);
}

TEST(CalcInterpreterTest, Visit_Unary_Num) {
  auto ast_const = std::make_shared<MockAstConst>(ConstType::kInteger, MockToken("3"));
  EXPECT_CALL(*ast_const, Accept(_)).WillOnce([&](IAstVisitor<MockMakeSharedPtr, MockToken>& value) { value.Visit(*ast_const); });

  auto ast_unary = make_shared<MockAstUnaryOp>(MockTokenId::kMinus, ast_const);
  EXPECT_CALL(*ast_unary, Accept(_)).WillOnce([&](IAstVisitor<MockMakeSharedPtr, MockToken>& value) {
    value.Visit(*ast_unary);  //
  });

  CalcInterpreter<MockMakeSharedPtr, MockToken> CalcInterpreter;
  auto res = CalcInterpreter.Interpret(ast_unary);
  EXPECT_EQ("-3", res.expr);
}

TEST(CalcInterpreterTest, Visit_BinOp_Add_2Factors) {
  auto ast_const3 = std::make_shared<MockAstConst>(ConstType::kInteger, MockToken("3"));
  EXPECT_CALL(*ast_const3, Accept(_)).WillOnce([&](IAstVisitor<MockMakeSharedPtr, MockToken>& value) {
    value.Visit(*ast_const3);  //
  });

  auto ast_const5 = std::make_shared<MockAstConst>(ConstType::kInteger, MockToken("5"));
  EXPECT_CALL(*ast_const5, Accept(_)).WillOnce([&](IAstVisitor<MockMakeSharedPtr, MockToken>& value) {
    value.Visit(*ast_const5);  //
  });

  auto ast_bin_op = std::make_shared<MockAstBinaryOp>(MockTokenId::kPlus, ast_const3, ast_const5);
  EXPECT_CALL(*ast_bin_op, Accept(_)).WillOnce([&](IAstVisitor<MockMakeSharedPtr, MockToken>& value) {
    value.Visit(*ast_bin_op);  //
  });

  CalcInterpreter<MockMakeSharedPtr, MockToken> CalcInterpreter;
  auto res = CalcInterpreter.Interpret(ast_bin_op);
  EXPECT_EQ("8", res.expr);
}

TEST(CalcInterpreterTest, Visit_BinOp_Add_3Factors) {
  auto ast_const1 = std::make_shared<MockAstConst>(ConstType::kInteger, MockToken("1"));
  EXPECT_CALL(*ast_const1, Accept(_)).WillOnce([&](IAstVisitor<MockMakeSharedPtr, MockToken>& value) {
    value.Visit(*ast_const1);  //
  });

  auto ast_const2 = std::make_shared<MockAstConst>(ConstType::kInteger, MockToken("2"));
  EXPECT_CALL(*ast_const2, Accept(_)).WillOnce([&](IAstVisitor<MockMakeSharedPtr, MockToken>& value) {
    value.Visit(*ast_const2);  //
  });

  auto ast_const3 = std::make_shared<MockAstConst>(ConstType::kInteger, MockToken("3"));
  EXPECT_CALL(*ast_const3, Accept(_)).WillOnce([&](IAstVisitor<MockMakeSharedPtr, MockToken>& value) {
    value.Visit(*ast_const3);  //
  });

  auto ast_bin_op = std::make_shared<MockAstBinaryOp>(MockTokenId::kPlus, ast_const1, ast_const2);
  EXPECT_CALL(*ast_bin_op, Accept(_)).WillOnce([&](IAstVisitor<MockMakeSharedPtr, MockToken>& value) {
    value.Visit(*ast_bin_op);  //
  });
  auto ast_bin_op_2 = std::make_shared<MockAstBinaryOp>(MockTokenId::kPlus, ast_bin_op, ast_const3);
  EXPECT_CALL(*ast_bin_op_2, Accept(_)).WillOnce([&](IAstVisitor<MockMakeSharedPtr, MockToken>& value) {
    value.Visit(*ast_bin_op_2);  //
  });

  CalcInterpreter<MockMakeSharedPtr, MockToken> CalcInterpreter;
  auto res = CalcInterpreter.Interpret(ast_bin_op_2);
  EXPECT_EQ("6", res.expr);
}

TEST(CalcInterpreterTest, Visit_BinOp_Mul_2Factors) {
  auto ast_const3 = std::make_shared<MockAstConst>(ConstType::kInteger, MockToken("3"));
  EXPECT_CALL(*ast_const3, Accept(_)).WillOnce([&](IAstVisitor<MockMakeSharedPtr, MockToken>& value) {
    value.Visit(*ast_const3);  //
  });

  auto ast_const5 = std::make_shared<MockAstConst>(ConstType::kInteger, MockToken("5"));
  EXPECT_CALL(*ast_const5, Accept(_)).WillOnce([&](IAstVisitor<MockMakeSharedPtr, MockToken>& value) {
    value.Visit(*ast_const5);  //
  });

  auto ast_bin_op = std::make_shared<MockAstBinaryOp>(MockTokenId::kMultiply, ast_const3, ast_const5);
  EXPECT_CALL(*ast_bin_op, Accept(_)).WillOnce([&](IAstVisitor<MockMakeSharedPtr, MockToken>& value) {
    value.Visit(*ast_bin_op);  //
  });

  CalcInterpreter<MockMakeSharedPtr, MockToken> CalcInterpreter;
  auto res = CalcInterpreter.Interpret(ast_bin_op);
  EXPECT_EQ("15", res.expr);
}
