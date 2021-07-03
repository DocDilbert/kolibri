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

using MockToken = std::string;

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
  explicit MockAstConst(ConstType const_type, std::string value) : AstConst<MockMakeSharedPtr, MockToken>(const_type, value) {}
  MOCK_METHOD1(Accept, void(IAstVisitor<MockMakeSharedPtr, MockToken>& value));
};

class MockAstUnaryOp : public AstUnaryOp<MockMakeSharedPtr, MockToken> {
 public:
  explicit MockAstUnaryOp(UnaryOpType type, nonterm_type operand) : AstUnaryOp<MockMakeSharedPtr, MockToken>(type, operand) {}
  MOCK_METHOD1(Accept, void(IAstVisitor<MockMakeSharedPtr, MockToken>& value));
};

class MockAstBinaryOp : public AstBinaryOp<MockMakeSharedPtr, MockToken> {
 public:
  explicit MockAstBinaryOp(BinaryOpType type, nonterm_type operand_lhs, nonterm_type operand_rhs)
      : AstBinaryOp<MockMakeSharedPtr, MockToken>(type, operand_lhs, operand_rhs) {}
  MOCK_METHOD1(Accept, void(IAstVisitor<MockMakeSharedPtr, MockToken>& value));
};

TEST(CalcInterpreterTest, Visit_Num) {
  auto ast_const = std::make_shared<MockAstConst>(ConstType::kInteger, "3");

  EXPECT_CALL(*ast_const, Accept(_)).WillOnce([&](IAstVisitor<MockMakeSharedPtr, MockToken>& value) { value.Visit(*ast_const); });

  CalcInterpreter<MockMakeSharedPtr, MockToken> CalcInterpreter;
  auto res = CalcInterpreter.Interpret(ast_const);
  EXPECT_EQ("3", res.expr);
}

TEST(CalcInterpreterTest, Visit_Unary_Num) {
  auto ast_const = std::make_shared<MockAstConst>(ConstType::kInteger, "3");
  EXPECT_CALL(*ast_const, Accept(_)).WillOnce([&](IAstVisitor<MockMakeSharedPtr, MockToken>& value) { value.Visit(*ast_const); });

  auto ast_unary = make_shared<MockAstUnaryOp>(UnaryOpType::kNegativeOp, ast_const);
  EXPECT_CALL(*ast_unary, Accept(_)).WillOnce([&](IAstVisitor<MockMakeSharedPtr, MockToken>& value) {
    value.Visit(*ast_unary);  //
  });

  CalcInterpreter<MockMakeSharedPtr, MockToken> CalcInterpreter;
  auto res = CalcInterpreter.Interpret(ast_unary);
  EXPECT_EQ("-3", res.expr);
}


TEST(CalcInterpreterTest, Visit_BinOp_Add_2Factors) {

  auto ast_const3 = std::make_shared<MockAstConst>(ConstType::kInteger, "3");
  EXPECT_CALL(*ast_const3, Accept(_)).WillOnce([&](IAstVisitor<MockMakeSharedPtr, MockToken>& value) {
    value.Visit(*ast_const3);  //
  });

  auto ast_const5 = std::make_shared<MockAstConst>(ConstType::kInteger, "5");
  EXPECT_CALL(*ast_const5, Accept(_)).WillOnce([&](IAstVisitor<MockMakeSharedPtr, MockToken>& value) {
    value.Visit(*ast_const5);  //
  });

  auto ast_bin_op = std::make_shared<MockAstBinaryOp>(BinaryOpType::kAdd, ast_const3, ast_const5);
  EXPECT_CALL(*ast_bin_op, Accept(_)).WillOnce([&](IAstVisitor<MockMakeSharedPtr, MockToken>& value) {
    value.Visit(*ast_bin_op);  //
  });

  CalcInterpreter<MockMakeSharedPtr, MockToken> CalcInterpreter;
  auto res = CalcInterpreter.Interpret(ast_bin_op);
  EXPECT_EQ("8", res.expr);
}




TEST(CalcInterpreterTest, Visit_BinOp_Add_3Factors) {
  auto ast_const1 = std::make_shared<MockAstConst>(ConstType::kInteger, "1");
  EXPECT_CALL(*ast_const1, Accept(_)).WillOnce([&](IAstVisitor<MockMakeSharedPtr, MockToken>& value) {
    value.Visit(*ast_const1);  //
  });
  
  auto ast_const2 = std::make_shared<MockAstConst>(ConstType::kInteger, "2");
  EXPECT_CALL(*ast_const2, Accept(_)).WillOnce([&](IAstVisitor<MockMakeSharedPtr, MockToken>& value) {
    value.Visit(*ast_const2);  //
  });
  
  auto ast_const3 = std::make_shared<MockAstConst>(ConstType::kInteger, "3");
  EXPECT_CALL(*ast_const3, Accept(_)).WillOnce([&](IAstVisitor<MockMakeSharedPtr, MockToken>& value) {
    value.Visit(*ast_const3);  //
  });

  auto ast_bin_op = std::make_shared<MockAstBinaryOp>(BinaryOpType::kAdd, ast_const1, ast_const2);
  EXPECT_CALL(*ast_bin_op, Accept(_)).WillOnce([&](IAstVisitor<MockMakeSharedPtr, MockToken>& value) {
    value.Visit(*ast_bin_op);  //
  });
  auto ast_bin_op_2 = std::make_shared<MockAstBinaryOp>(BinaryOpType::kAdd, ast_bin_op, ast_const3);
  EXPECT_CALL(*ast_bin_op_2, Accept(_)).WillOnce([&](IAstVisitor<MockMakeSharedPtr, MockToken>& value) {
    value.Visit(*ast_bin_op_2);  //
  });

  CalcInterpreter< MockMakeSharedPtr, MockToken> CalcInterpreter;
  auto res = CalcInterpreter.Interpret(ast_bin_op_2);
  EXPECT_EQ("6", res.expr);
}

TEST(CalcInterpreterTest, Visit_BinOp_Mul_2Factors) {

  auto ast_const3 = std::make_shared<MockAstConst>(ConstType::kInteger, "3");
  EXPECT_CALL(*ast_const3, Accept(_)).WillOnce([&](IAstVisitor<MockMakeSharedPtr, MockToken>& value) {
    value.Visit(*ast_const3);  //
  });
  
  auto ast_const5 = std::make_shared<MockAstConst>(ConstType::kInteger, "5");
  EXPECT_CALL(*ast_const5, Accept(_)).WillOnce([&](IAstVisitor<MockMakeSharedPtr, MockToken>& value) {
    value.Visit(*ast_const5);  //
  });

  auto ast_bin_op = std::make_shared<MockAstBinaryOp>(BinaryOpType::kMul, ast_const3, ast_const5);
  EXPECT_CALL(*ast_bin_op, Accept(_)).WillOnce([&](IAstVisitor<MockMakeSharedPtr, MockToken>& value) {
    value.Visit(*ast_bin_op);  //
  });

  CalcInterpreter<MockMakeSharedPtr, MockToken> CalcInterpreter;
  auto res = CalcInterpreter.Interpret(ast_bin_op);
  EXPECT_EQ("15", res.expr);
}

