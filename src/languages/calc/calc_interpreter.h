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
    void Visit(AstConst<TMakeType, term_type>& ast) override { return_int_ = std::stoi(std::string(ast.GetValue().GetValue())); }
    void Visit(AstProgram<TMakeType, term_type>& ast) override {}
    void Visit(AstBlock<TMakeType, term_type>& ast) override {}
    void Visit(AstVariableDeclaration<TMakeType, term_type>& ast) override {}
    void Visit(AstNop<TMakeType, term_type>& ast) override {}
    void Visit(AstId<TMakeType, term_type>& ast) override {}
    void Visit(AstCompoundStatement<TMakeType, term_type>& ast) override {}
    void Visit(AstUnaryOp<TMakeType, term_type>& ast) override {
      auto operand = ast.GetOperand();
      operand->Accept(*this);
  

      switch (ast.GetOperator().GetId()) {
        case term_type::id_type::kPlus:
          return_int_ = +return_int_;
          break;
        case term_type::id_type::kMinus:
          return_int_ = -return_int_;
          break;

        default:
          break;
      }
    }
    void Visit(AstBinaryOp<TMakeType, term_type>& ast) override {

      ast.GetOperandLhs()->Accept(*this);
      auto lhs_res = return_int_; 
      ast.GetOperandRhs()->Accept(*this);
      auto rhs_res = return_int_; 

      switch (ast.GetOperator().GetId()) {
        case term_type::id_type::kPlus:
          return_int_ = lhs_res + rhs_res;
          break;
        case term_type::id_type::kMinus:
          return_int_ = lhs_res - rhs_res;
          break;
        case term_type::id_type::kMultiply:
          return_int_ = lhs_res * rhs_res;
          break;
        case term_type::id_type::kDiv:
          return_int_ = lhs_res / rhs_res;
          break;
        default:
          break;
      }
    }

    int GetReturnInt() { return return_int_; }

   private:
    int return_int_;
  };

  RuleResult Interpret(nonterm_type node) {
    Visitor visitor;

    node->Accept(visitor);
    std::string expr = std::to_string(visitor.GetReturnInt());

    return {expr, false, ""};
  }
};

}  // namespace calc
}  // namespace languages
#endif