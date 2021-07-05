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
    VisitorReturn Visit(AstConst<TMakeType, term_type>& ast) override {
      int return_int = std::stoi(std::string(ast.GetValue().GetValue()));
      return VisitorReturn(return_int);
    }
    VisitorReturn Visit(AstProgram<TMakeType, term_type>& ast) override { return VisitorReturn(); }
    VisitorReturn Visit(AstBlock<TMakeType, term_type>& ast) override { return VisitorReturn(); }
    VisitorReturn Visit(AstVariableDeclaration<TMakeType, term_type>& ast) override { return VisitorReturn(); }
    VisitorReturn Visit(AstNop<TMakeType, term_type>& ast) override { return VisitorReturn(); }
    VisitorReturn Visit(AstId<TMakeType, term_type>& ast) override { return VisitorReturn(); }
    VisitorReturn Visit(AstCompoundStatement<TMakeType, term_type>& ast) override { return VisitorReturn(); }
    VisitorReturn Visit(AstUnaryOp<TMakeType, term_type>& ast) override {
      auto operand = ast.GetOperand();
      auto res = operand->Accept(*this);
      int return_int = 0;
      switch (ast.GetOperator().GetId()) {
        case term_type::id_type::kPlus:
          return_int = res.return_int;
          break;
        case term_type::id_type::kMinus:
          return_int = -res.return_int;
          break;

        default:
          break;
      }
      return VisitorReturn(return_int);
    }
    VisitorReturn Visit(AstBinaryOp<TMakeType, term_type>& ast) override {
      auto lhs_res = ast.GetOperandLhs()->Accept(*this);
      auto rhs_res = ast.GetOperandRhs()->Accept(*this);
      int return_int = 0;
      switch (ast.GetOperator().GetId()) {
        case term_type::id_type::kPlus:
          return_int = lhs_res.return_int + rhs_res.return_int;
          break;
        case term_type::id_type::kMinus:
          return_int = lhs_res.return_int - rhs_res.return_int;
          break;
        case term_type::id_type::kMultiply:
          return_int = lhs_res.return_int * rhs_res.return_int;
          break;
        case term_type::id_type::kDiv:
          return_int = lhs_res.return_int / rhs_res.return_int;
          break;
        default:
          break;
      }
      return VisitorReturn(return_int);
    }
  };

  RuleResult Interpret(nonterm_type node) {
    Visitor visitor;

    auto res = node->Accept(visitor);
    std::string expr = std::to_string(res.return_int);

    return {expr, false, ""};
  }
};

}  // namespace calc
}  // namespace languages
#endif