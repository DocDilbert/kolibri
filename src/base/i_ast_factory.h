#ifndef KOLIBRI_SRC_I_AST_FACTORY_H_
#define KOLIBRI_SRC_I_AST_FACTORY_H_

#include <string>
#include <vector>

#include "base/ast_id.h"

namespace base {

template <typename TNonTerm, typename TTerm>
class IAstFactory {
 public:
  using nonterm_type = TNonTerm;
  using term_type = TTerm;

  virtual nonterm_type CreateRaw(term_type term) = 0;
  virtual nonterm_type CreateRawList(std::vector<nonterm_type> const& var_decls) = 0;
  virtual nonterm_type CreateNull() = 0;
  virtual nonterm_type CreateNop() = 0;
  virtual nonterm_type CreateProgram(nonterm_type left, nonterm_type right) = 0;
  virtual nonterm_type CreateBlock(std::vector<nonterm_type> const& var_decls, nonterm_type compound_statement) = 0;
  virtual nonterm_type CreateId(std::string name) = 0;
  virtual nonterm_type CreateConst(ConstType const_type, std::string value) = 0;
  virtual nonterm_type CreateCompoundStatement(std::vector<nonterm_type> statements) = 0;
  virtual nonterm_type CreateUnaryOp(term_type oper, nonterm_type operand) = 0;
  virtual nonterm_type CreateBinaryOp(nonterm_type left, term_type oper, nonterm_type right) = 0;
  virtual nonterm_type CreateVariableDeclaration(term_type id, term_type type) = 0;
};

}  // namespace base
#endif