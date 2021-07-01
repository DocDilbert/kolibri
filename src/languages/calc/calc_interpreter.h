#ifndef KOLIBRI_SRC_CALC_INTERPRETER_H_
#define KOLIBRI_SRC_CALC_INTERPRETER_H_

#include <string>

#include "base/i_ast_visitor.h"

namespace languages {
namespace calc {

template <typename TNormType, typename TTerm>
class CalcInterpreter {
 public:
  using nonterm_type = TNormType;
  using term_type = TTerm;

  struct RuleResult {
    std::string expr;
    bool error;
    const char* error_msg;
  };

  class Visitor : public base::IAstVisitor<nonterm_type, term_type> {
   public:
    void VisitIntegerConst(base::ConstType const_type, std::string value) override { num_ = std::stoi(value); }
    void VisitProgram(nonterm_type program_id, nonterm_type program) override {}
    void VisitBlock(std::vector<nonterm_type> const& var_decls, nonterm_type compound_statement) override {}
    void VisitVariableDeclaration(term_type id, term_type type) override {}
    void VisitNop() override {}
    void VisitId(std::string) override {}
    void VisitCompoundStatement(std::vector<nonterm_type> const& statements) override {}
    void VisitUnaryOp(base::UnaryOpType type, nonterm_type operand) override {
      Visitor visitor;

      operand->Visit(visitor);

      switch (type) {
        case base::UnaryOpType::kPositiveOp:
          num_ = visitor.GetNum();
          break;
        case base::UnaryOpType::kNegativeOp:
          num_ = -visitor.GetNum();
          break;

        default:
          break;
      }
    }
    void VisitBinaryOp(base::BinaryOpType op, nonterm_type operand_lhs, nonterm_type operand_rhs) override {
      Visitor visitor_lhs;
      Visitor visitor_rhs;

      operand_lhs->Visit(visitor_lhs);
      operand_rhs->Visit(visitor_rhs);

      switch (op) {
        case base::BinaryOpType::kAdd:
          num_ = visitor_lhs.GetNum() + visitor_rhs.GetNum();
          break;
        case base::BinaryOpType::kSub:
          num_ = visitor_lhs.GetNum() - visitor_rhs.GetNum();
          break;
        case base::BinaryOpType::kMul:
          num_ = visitor_lhs.GetNum() * visitor_rhs.GetNum();
          break;
        case base::BinaryOpType::kIntegerDiv:
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

    node->Visit(visitor);
    std::string expr = std::to_string(visitor.GetNum());

    return {expr, false, ""};
  }
};

}  // namespace calc
}  // namespace languages
#endif