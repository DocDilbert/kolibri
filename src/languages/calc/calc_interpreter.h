#ifndef KOLIBRI_SRC_CALC_INTERPRETER_H_
#define KOLIBRI_SRC_CALC_INTERPRETER_H_

#include <string>

#include "languages/ast.h"
#include "languages/i_ast_visitor.h"

namespace languages {
namespace calc {

template <template <class> class TMakeType, typename TTerm>
class CalcInterpreter {
 public:
  using nonterm_type = typename TMakeType<Ast<TMakeType, TTerm>>::type;
  using term_type = TTerm;

  struct RuleResult {
    std::string expr;
    bool error;
    const char* error_msg;
  };

  class Visitor : public IAstVisitor<TMakeType, term_type> {
   public:
    void Visit(AstConst<TMakeType, term_type>& ast) override { num_ = std::stoi(std::string(ast.GetValue().GetValue())); }
    void Visit(AstProgram<TMakeType, term_type>& ast) override {}
    void Visit(AstBlock<TMakeType, term_type>& ast) override {}
    void Visit(AstVariableDeclaration<TMakeType, term_type>& ast) override {}
    void Visit(AstNop<TMakeType, term_type>& ast) override {}
    void Visit(AstId<TMakeType, term_type>& ast) override {}
    void Visit(AstCompoundStatement<TMakeType, term_type>& ast) override {}
    void Visit(AstUnaryOp<TMakeType, term_type>& ast) override {
      Visitor visitor;
      auto operand = ast.GetOperand();
      operand->Accept(visitor);
      // auto op = ast.GetOperator().GetId();

      switch (ast.GetOperator().GetId()) {
        case term_type::id_type::kPlus:
          num_ = visitor.GetNum();
          break;
        case term_type::id_type::kMinus:
          num_ = -visitor.GetNum();
          break;

        default:
          break;
      }
    }
    void Visit(AstBinaryOp<TMakeType, term_type>& ast) override {
      Visitor visitor_lhs;
      Visitor visitor_rhs;

      ast.GetOperandLhs()->Accept(visitor_lhs);
      ast.GetOperandRhs()->Accept(visitor_rhs);

      switch (ast.GetOperator().GetId()) {
        case term_type::id_type::kPlus:
          num_ = visitor_lhs.GetNum() + visitor_rhs.GetNum();
          break;
        case term_type::id_type::kMinus:
          num_ = visitor_lhs.GetNum() - visitor_rhs.GetNum();
          break;
        case term_type::id_type::kMultiply:
          num_ = visitor_lhs.GetNum() * visitor_rhs.GetNum();
          break;
        case term_type::id_type::kDiv:
          num_ = visitor_lhs.GetNum() / visitor_rhs.GetNum();
          break;
        default:
          break;
      }
    }

    int GetNum() { return num_; }

   private:
    int num_;
  };

  RuleResult Interpret(nonterm_type node) {
    Visitor visitor;

    node->Accept(visitor);
    std::string expr = std::to_string(visitor.GetNum());

    return {expr, false, ""};
  }
};

}  // namespace calc
}  // namespace languages
#endif