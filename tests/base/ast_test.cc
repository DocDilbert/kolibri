#include "base/ast.h"
#include "base/ast_types.h" // TODO: Diese Abhängigkeit muss entfernt werden.
#include <gmock/gmock.h>  // Brings in Google Mock.
#include <gtest/gtest.h>

#include <memory>

using namespace base;
using namespace std;

using ::testing::_;
using ::testing::Ref;
using ::testing::Return;

using MockToken = std::string;


class MockAstVisitor : public IAstVisitor<std::shared_ptr<Ast<MakeShared, MockToken>>, MockToken> {
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
  shared_ptr<Ast<MakeShared, MockToken>> num1 = std::make_shared<AstConst<MakeShared, MockToken>>(ConstType::INTEGER, "2");
  EXPECT_CALL(mock_visitor, VisitIntegerConst(ConstType::INTEGER, "2")).Times(1);
  EXPECT_CALL(mock_visitor, VisitId).Times(0);
  EXPECT_CALL(mock_visitor, VisitNop).Times(0);
  EXPECT_CALL(mock_visitor, VisitUnaryOp).Times(0);
  EXPECT_CALL(mock_visitor, VisitBinaryOp).Times(0);
  num1->Visit(mock_visitor);
}

TEST(AstTest, BinOp) {
  MockAstVisitor mock_visitor;
  shared_ptr<Ast<MakeShared, MockToken>> num1 = std::make_shared<AstConst<MakeShared, MockToken>>(ConstType::INTEGER, "2");
  shared_ptr<Ast<MakeShared, MockToken>> num2 = std::make_shared<AstConst<MakeShared, MockToken>>(ConstType::INTEGER, "3");
  auto& n = *num1;
  shared_ptr<Ast<MakeShared, MockToken>> op = std::make_shared<AstBinaryOp<MakeShared, MockToken>>(BinaryOpType::INTEGER_DIV, num1, num2);
  EXPECT_CALL(mock_visitor, VisitIntegerConst).Times(0);
  EXPECT_CALL(mock_visitor, VisitId).Times(0);
  EXPECT_CALL(mock_visitor, VisitNop).Times(0);
  EXPECT_CALL(mock_visitor, VisitUnaryOp).Times(0);
  EXPECT_CALL(mock_visitor, VisitBinaryOp(BinaryOpType::INTEGER_DIV, num1, num2)).Times(1);
  op->Visit(mock_visitor);
}
