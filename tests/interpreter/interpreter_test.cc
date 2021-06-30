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

class MockType {
 public:
  using value_type = MockType;
  using reference_type = MockType&;
  using pointer_type = std::shared_ptr<MockType>;

  MOCK_METHOD1(Visit, void(IAstVisitor<std::shared_ptr<value_type>, MockToken>& value));
};

TEST(CalcInterpreterTest, Visit_Num) {
  auto mock_type = std::make_shared<MockType>();
  EXPECT_CALL(*mock_type, Visit(_)).WillOnce([](IAstVisitor<std::shared_ptr<MockType>, MockToken>& value) {
    value.VisitIntegerConst(ConstType::INTEGER, "3");
  });
  CalcInterpreter<std::shared_ptr<MockType>, MockToken> CalcInterpreter;
  auto res = CalcInterpreter.Interpret(mock_type);
  EXPECT_EQ("3", res.expr);
}

TEST(CalcInterpreterTest, Visit_Unary_Num) {
  auto mock_type = std::make_shared<MockType>();
  EXPECT_CALL(*mock_type, Visit(_)).WillOnce([](IAstVisitor<std::shared_ptr<MockType>, MockToken>& value) {
    value.VisitIntegerConst(ConstType::INTEGER, "3");
  });

  auto mock_type_op = std::make_shared<MockType>();
  EXPECT_CALL(*mock_type_op, Visit(_)).WillOnce([&](IAstVisitor<std::shared_ptr<MockType>, MockToken>& value) {
    value.VisitUnaryOp(UnaryOpType::NEGATIVE_OP, mock_type);  //
  });

  CalcInterpreter<std::shared_ptr<MockType>, MockToken> CalcInterpreter;
  auto res = CalcInterpreter.Interpret(mock_type_op);
  EXPECT_EQ("-3", res.expr);
}

TEST(CalcInterpreterTest, Visit_BinOp_Add_2Factors) {
  auto mock_type_num1 = std::make_shared<MockType>();
  EXPECT_CALL(*mock_type_num1, Visit(_)).WillOnce([](IAstVisitor<std::shared_ptr<MockType>, MockToken>& value) {
    value.VisitIntegerConst(ConstType::INTEGER, "3");  //
  });
  auto mock_type_num2 = std::make_shared<MockType>();
  EXPECT_CALL(*mock_type_num2, Visit(_)).WillOnce([](IAstVisitor<std::shared_ptr<MockType>, MockToken>& value) {
    value.VisitIntegerConst(ConstType::INTEGER, "5");  //
  });
  auto mock_type_op = std::make_shared<MockType>();
  EXPECT_CALL(*mock_type_op, Visit(_)).WillOnce([&](IAstVisitor<std::shared_ptr<MockType>, MockToken>& value) {
    value.VisitBinaryOp(BinaryOpType::ADD, mock_type_num1, mock_type_num2);  //
  });

  CalcInterpreter<std::shared_ptr<MockType>, MockToken> CalcInterpreter;
  auto res = CalcInterpreter.Interpret(mock_type_op);
  EXPECT_EQ("8", res.expr);
}

TEST(CalcInterpreterTest, Visit_BinOp_Add_3Factors) {
  auto mock_type_num1 = std::make_shared<MockType>();
  EXPECT_CALL(*mock_type_num1, Visit(_)).WillOnce([](IAstVisitor<std::shared_ptr<MockType>, MockToken>& value) {
    value.VisitIntegerConst(ConstType::INTEGER, "1");  //
  });
  auto mock_type_num2 = std::make_shared<MockType>();
  EXPECT_CALL(*mock_type_num2, Visit(_)).WillOnce([](IAstVisitor<std::shared_ptr<MockType>, MockToken>& value) {
    value.VisitIntegerConst(ConstType::INTEGER, "2");  //
  });
  auto mock_type_num3 = std::make_shared<MockType>();
  EXPECT_CALL(*mock_type_num3, Visit(_)).WillOnce([](IAstVisitor<std::shared_ptr<MockType>, MockToken>& value) {
    value.VisitIntegerConst(ConstType::INTEGER, "3");  //
  });
  auto mock_type_op1 = std::make_shared<MockType>();
  EXPECT_CALL(*mock_type_op1, Visit(_)).WillOnce([&](IAstVisitor<std::shared_ptr<MockType>, MockToken>& value) {
    value.VisitBinaryOp(BinaryOpType::ADD, mock_type_num1, mock_type_num2);  //
  });
  auto mock_type_op2 = std::make_shared<MockType>();
  EXPECT_CALL(*mock_type_op2, Visit(_)).WillOnce([&](IAstVisitor<std::shared_ptr<MockType>, MockToken>& value) {
    value.VisitBinaryOp(BinaryOpType::ADD, mock_type_op1, mock_type_num3);  //
  });

  CalcInterpreter<std::shared_ptr<MockType>, MockToken> CalcInterpreter;
  auto res = CalcInterpreter.Interpret(mock_type_op2);
  EXPECT_EQ("6", res.expr);
}

TEST(CalcInterpreterTest, Visit_BinOp_Mul_2Factors) {
  auto mock_type_num1 = std::make_shared<MockType>();
  EXPECT_CALL(*mock_type_num1, Visit(_)).WillOnce([](IAstVisitor<std::shared_ptr<MockType>, MockToken>& value) {
    value.VisitIntegerConst(ConstType::INTEGER, "3");  //
  });
  auto mock_type_num2 = std::make_shared<MockType>();
  EXPECT_CALL(*mock_type_num2, Visit(_)).WillOnce([](IAstVisitor<std::shared_ptr<MockType>, MockToken>& value) {
    value.VisitIntegerConst(ConstType::INTEGER, "5");  //
  });
  auto mock_type_op = std::make_shared<MockType>();
  EXPECT_CALL(*mock_type_op, Visit(_)).WillOnce([&](IAstVisitor<std::shared_ptr<MockType>, MockToken>& value) {
    value.VisitBinaryOp(BinaryOpType::MUL, mock_type_num1, mock_type_num2);  //
  });

  CalcInterpreter<std::shared_ptr<MockType>, MockToken> CalcInterpreter;
  auto res = CalcInterpreter.Interpret(mock_type_op);
  EXPECT_EQ("15", res.expr);
}
