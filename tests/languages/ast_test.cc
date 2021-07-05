#include "languages/ast.h"

#include <gmock/gmock.h>  // Brings in Google Mock.
#include <gtest/gtest.h>

#include <memory>

using namespace languages;
using namespace std;

using ::testing::_;
using ::testing::An;
using ::testing::MakeAction;
using ::testing::Matcher;
using ::testing::Ref;
using ::testing::Return;

using MockToken = std::string;

template <typename T>
class MockMakePtr {
 public:
  using type = T *;
};

class MockAstVisitor : public IAstVisitor<MockMakePtr, MockToken> {
 public:
  MOCK_METHOD(VisitorReturn, Visit, ((AstNop<MockMakePtr, MockToken> & ast)), (override));
  MOCK_METHOD(VisitorReturn, Visit, ((AstProgram<MockMakePtr, MockToken> & ast)), (override));
  MOCK_METHOD(VisitorReturn, Visit, ((AstBlock<MockMakePtr, MockToken> & ast)), (override));
  MOCK_METHOD(VisitorReturn, Visit, ((AstVariableDeclaration<MockMakePtr, MockToken> & ast)), (override));
  MOCK_METHOD(VisitorReturn, Visit, ((AstConst<MockMakePtr, MockToken> & ast)), (override));
  MOCK_METHOD(VisitorReturn, Visit, ((AstId<MockMakePtr, MockToken> & ast)), (override));
  MOCK_METHOD(VisitorReturn, Visit, ((AstCompoundStatement<MockMakePtr, MockToken> & ast)), (override));
  MOCK_METHOD(VisitorReturn, Visit, ((AstUnaryOp<MockMakePtr, MockToken> & ast)), (override));
  MOCK_METHOD(VisitorReturn, Visit, ((AstBinaryOp<MockMakePtr, MockToken> & ast)), (override));
};

TEST(AstTest, Num) {
  MockAstVisitor mock_visitor;
  auto num1 = AstConst<MockMakePtr, MockToken>(ConstType::kInteger, "2");
  EXPECT_CALL(mock_visitor, Visit(Matcher<AstConst<MockMakePtr, MockToken> &>(Ref(num1)))).Times(1);

  num1.Accept(mock_visitor);
}

TEST(AstTest, BinOp) {
  MockAstVisitor mock_visitor;
  auto num1 = AstConst<MockMakePtr, MockToken>(ConstType::kInteger, "2");
  auto num2 = AstConst<MockMakePtr, MockToken>(ConstType::kInteger, "3");
  auto op = AstBinaryOp<MockMakePtr, MockToken>(&num1, "BinaryOp", &num2);

  EXPECT_CALL(mock_visitor, Visit(Matcher<AstBinaryOp<MockMakePtr, MockToken> &>(Ref(op)))).Times(1);

  op.Accept(mock_visitor);
}
