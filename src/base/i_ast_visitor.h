#ifndef INTERTEST_SRC_I_AST_VISITOR_H_
#define INTERTEST_SRC_I_AST_VISITOR_H_

#include "base/ast_id.h"

namespace base {

template <typename TNonTerm, typename TTerm>
class IAstVisitor {
 public:
  using nonterm_type = TNonTerm;
  using term_type = TTerm;

  virtual void VisitId(std::string name) = 0;
  virtual void VisitNop() = 0;
  virtual void VisitProgram(nonterm_type program_id, nonterm_type program) = 0;
  virtual void VisitBlock(std::vector<nonterm_type> const& var_decls, nonterm_type compound_statement) = 0;
  virtual void VisitVariableDeclaration(term_type id, term_type type) = 0;
  virtual void VisitIntegerConst(ConstType const_type, std::string value) = 0;
  virtual void VisitCompoundStatement(std::vector<nonterm_type> const& statements) = 0;
  virtual void VisitUnaryOp(UnaryOpType type, nonterm_type operand) = 0;
  virtual void VisitBinaryOp(BinaryOpType op, nonterm_type operand_lhs, nonterm_type operand_rhs) = 0;
};
}  // namespace base
#endif