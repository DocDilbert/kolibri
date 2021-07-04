#ifndef KOLIBRI_SRC_AST_FACTORY_H_
#define KOLIBRI_SRC_AST_FACTORY_H_

#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "languages/ast.h"
#include "languages/ast_types.h"
#include "languages/i_ast_factory.h"
namespace languages {

template <typename TNonTerm, typename TTerm>
class AstFactory : public IAstFactory<TNonTerm, TTerm> {
 public:
  using nonterm_type = TNonTerm;
  using term_type = TTerm;

  virtual nonterm_type CreateNull() override { return nullptr; }

  virtual nonterm_type CreateNop() override { return std::make_shared<AstNop<MakeShared, TTerm>>(); }

  virtual nonterm_type CreateProgram(nonterm_type left, nonterm_type right) override { return std::make_shared<AstProgram<MakeShared, TTerm>>(left, right); }

  virtual nonterm_type CreateBlock(std::vector<nonterm_type> const& var_decls, nonterm_type compound_statement) override {
    return std::make_shared<AstBlock<MakeShared, TTerm>>(var_decls, compound_statement);
  }

  virtual nonterm_type CreateId(term_type name) override { return std::make_shared<AstId<MakeShared, TTerm>>(name); }

  virtual nonterm_type CreateRaw(term_type term) override { return std::make_shared<AstRaw<MakeShared, TTerm>>(term); }

  virtual nonterm_type CreateConst(ConstType const_type, term_type value) override { return std::make_shared<AstConst<MakeShared, TTerm>>(const_type, value); }

  virtual nonterm_type CreateCompoundStatement(std::vector<nonterm_type> statements) override {
    return std::make_shared<AstCompoundStatement<MakeShared, TTerm>>(statements);
  }

  virtual nonterm_type CreateUnaryOp(term_type oper, nonterm_type operand) override { return std::make_shared<AstUnaryOp<MakeShared, TTerm>>(oper, operand); }

  virtual nonterm_type CreateBinaryOp(nonterm_type operand_lhs, term_type oper, nonterm_type operand_rhs) override {
    return std::make_shared<AstBinaryOp<MakeShared, TTerm>>(operand_lhs, oper, operand_rhs);
  }

  virtual nonterm_type CreateVariableDeclaration(term_type id, term_type type) override {
    return std::make_shared<AstVariableDeclaration<MakeShared, TTerm>>(id, type);
  }

  virtual nonterm_type CreateRawList(std::vector<TNonTerm> const& var_decls) override { return std::make_shared<AstRawList<MakeShared, TTerm>>(var_decls); }
};
}  // namespace languages
#endif