#include "base/ast.h"
#include <gmock/gmock.h>  // Brings in Google Mock.
#include <gtest/gtest.h>

#include <memory>

using namespace base;
using namespace std;

using ::testing::_;
using ::testing::Ref;
using ::testing::Return;

using MockToken = std::string;

template <typename T>
class MockMakePtr {
 public:
  using type = T*;
};

class MockAstVisitor : public IAstVisitor<Ast<MockMakePtr, MockToken>*, MockToken> {
 public:
  MOCK_METHOD0(VisitNop, void());
  MOCK_METHOD2(VisitProgram, void(nonterm_type left, nonterm_type right));
  MOCK_METHOD2(VisitBlock, void(std::vector<nonterm_type> const& var_decls, nonterm_type compound_statement));
  MOCK_METHOD2(VisitVariableDeclaration, void(term_type id, term_type type));
  MOCK_METHOD2(VisitIntegerConst, void(base::ConstType const_type, std::string value));
  MOCK_METHOD1(VisitId, void(std::string));
  MOCK_METHOD1(VisitCompoundStatement, void(std::vector<nonterm_type> const& statements));
  MOCK_METHOD2(VisitUnaryOp, void(UnaryOpType type, nonterm_type operand));
  MOCK_METHOD3(VisitBinaryOp, void(BinaryOpType type, nonterm_type left, nonterm_type right));
};

TEST(AstTest, Num) {
  MockAstVisitor mock_visitor;
  auto num1 = AstConst<MockMakePtr, MockToken>(ConstType::kInteger, "2");
  EXPECT_CALL(mock_visitor, VisitIntegerConst(ConstType::kInteger, "2")).Times(1);
  EXPECT_CALL(mock_visitor, VisitId).Times(0);
  EXPECT_CALL(mock_visitor, VisitNop).Times(0);
  EXPECT_CALL(mock_visitor, VisitUnaryOp).Times(0);
  EXPECT_CALL(mock_visitor, VisitBinaryOp).Times(0);
  num1.Accept(mock_visitor);
}

TEST(AstTest, BinOp) {
  MockAstVisitor mock_visitor;
  auto num1 = AstConst<MockMakePtr, MockToken>(ConstType::kInteger, "2");
  auto num2 = AstConst<MockMakePtr, MockToken>(ConstType::kInteger, "3");
  auto op = AstBinaryOp<MockMakePtr, MockToken>(BinaryOpType::kIntegerDiv, &num1, &num2);

  EXPECT_CALL(mock_visitor, VisitIntegerConst).Times(0);
  EXPECT_CALL(mock_visitor, VisitId).Times(0);
  EXPECT_CALL(mock_visitor, VisitNop).Times(0);
  EXPECT_CALL(mock_visitor, VisitUnaryOp).Times(0);
  EXPECT_CALL(mock_visitor, VisitBinaryOp(BinaryOpType::kIntegerDiv, &num1, &num2)).Times(1);
  op.Accept(mock_visitor);
}
