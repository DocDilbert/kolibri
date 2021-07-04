#ifndef KOLIBRI_SRC_AST_H_
#define KOLIBRI_SRC_AST_H_

#include <memory>
#include <string>
#include <vector>

#include "languages/ast_id.h"
#include "languages/i_ast_visitor.h"

namespace languages {

enum class AstTypeId {
  kAstNop,
  kAstRawType,
  kAstRawListType,
  kAstId,
  kAstConst,
  kAstCompoundStatement,
  kAstUnaryOp,
  kAstBinaryOp,
  kAstProgram,
  kAstVariableDeclaration
};

template <template <class> class TMakeType, typename TTerm>
class Ast {
 public:
  using term_type = TTerm;
  using nonterm_type = typename TMakeType<Ast<TMakeType, TTerm>>::type;

  virtual AstTypeId GetTypeId() = 0;
  virtual void Accept(IAstVisitor<TMakeType, term_type>& visitor) = 0;
};

template <template <class> class TMakeType, typename TTerm>
class AstNop : public Ast<TMakeType, TTerm> {
 public:
  using nonterm_type = typename Ast<TMakeType, TTerm>::nonterm_type;
  using term_type = typename Ast<TMakeType, TTerm>::term_type;

  explicit AstNop() {}
  AstTypeId GetTypeId() override { return AstTypeId::kAstNop; }

  void Accept(IAstVisitor<TMakeType, term_type>& visitor) override { visitor.Visit(*this); }
};

template <template <class> class TMakeType, typename TTerm>
class AstId : public Ast<TMakeType, TTerm> {
 public:
  using nonterm_type = typename Ast<TMakeType, TTerm>::nonterm_type;
  using term_type = typename Ast<TMakeType, TTerm>::term_type;

  explicit AstId(term_type name) { name_ = name; }
  AstTypeId GetTypeId() override { return AstTypeId::kAstId; }
  void Accept(IAstVisitor<TMakeType, term_type>& visitor) override { visitor.Visit(*this); }

  term_type GetName() { return name_; }

 private:
  term_type name_;
};

template <template <class> class TMakeType, typename TTerm>
class AstRaw : public Ast<TMakeType, TTerm> {
 public:
  using nonterm_type = typename Ast<TMakeType, TTerm>::nonterm_type;
  using term_type = typename Ast<TMakeType, TTerm>::term_type;
  explicit AstRaw(TTerm term) { term_ = term; }
  AstTypeId GetTypeId() override { return AstTypeId::kAstRawType; }
  TTerm GetTerm() { return term_; }
  void Accept(IAstVisitor<TMakeType, term_type>& visitor) override {}

 private:
  TTerm term_;
};

template <template <class> class TMakeType, typename TTerm>
class AstRawList : public Ast<TMakeType, TTerm> {
 public:
  using nonterm_type = typename Ast<TMakeType, TTerm>::nonterm_type;
  using term_type = typename Ast<TMakeType, TTerm>::term_type;

  explicit AstRawList(std::vector<nonterm_type> const& nonterms) : nonterms_(nonterms) {}

  AstTypeId GetTypeId() override { return AstTypeId::kAstRawListType; }

  std::vector<nonterm_type> Get() { return nonterms_; }

  void Accept(IAstVisitor<TMakeType, term_type>& visitor) override {}

 private:
  std::vector<nonterm_type> nonterms_;
};

template <template <class> class TMakeType, typename TTerm>
class AstConst : public Ast<TMakeType, TTerm> {
 public:
  using nonterm_type = typename Ast<TMakeType, TTerm>::nonterm_type;
  using term_type = typename Ast<TMakeType, TTerm>::term_type;

  explicit AstConst(ConstType const_type, term_type value) : const_type_(const_type), value_(value) {}
  AstTypeId GetTypeId() override { return AstTypeId::kAstConst; }

  void Accept(IAstVisitor<TMakeType, term_type>& visitor) override { visitor.Visit(*this); }

  ConstType GetConstType() { return const_type_; }

  term_type GetValue() { return value_; }

 private:
  ConstType const_type_;
  term_type value_;
};

template <template <class> class TMakeType, typename TTerm>
class AstCompoundStatement : public Ast<TMakeType, TTerm> {
 public:
  using nonterm_type = typename Ast<TMakeType, TTerm>::nonterm_type;
  using term_type = typename Ast<TMakeType, TTerm>::term_type;

  explicit AstCompoundStatement(std::vector<nonterm_type> statements) { statements_ = statements; }
  AstTypeId GetTypeId() override { return AstTypeId::kAstCompoundStatement; }
  std::vector<nonterm_type> GetStatements() { return statements_; }

  void Accept(IAstVisitor<TMakeType, term_type>& visitor) override { visitor.Visit(*this); }

 private:
  std::vector<nonterm_type> statements_;
};
template <template <class> class TMakeType, typename TTerm>
class AstUnaryOp : public Ast<TMakeType, TTerm> {
 public:
  using nonterm_type = typename Ast<TMakeType, TTerm>::nonterm_type;
  using term_type = typename Ast<TMakeType, TTerm>::term_type;

  explicit AstUnaryOp(term_type oper, nonterm_type operand) : operator_(oper), operand_(operand) {}
  AstTypeId GetTypeId() override { return AstTypeId::kAstUnaryOp; }

  void Accept(IAstVisitor<TMakeType, term_type>& visitor) override { visitor.Visit(*this); }

  term_type GetOperator() { return operator_; };
  nonterm_type GetOperand() { return operand_; }

 private:
  term_type operator_;
  nonterm_type operand_;
};

template <template <class> class TMakeType, typename TTerm>
class AstBinaryOp : public Ast<TMakeType, TTerm> {
 public:
  using nonterm_type = typename Ast<TMakeType, TTerm>::nonterm_type;
  using term_type = typename Ast<TMakeType, TTerm>::term_type;

  explicit AstBinaryOp(nonterm_type operand_lhs, term_type oper, nonterm_type operand_rhs)
      : operand_lhs_(operand_lhs), operator_(oper), operand_rhs_(operand_rhs) {}
  AstTypeId GetTypeId() override { return AstTypeId::kAstBinaryOp; }

  void Accept(IAstVisitor<TMakeType, term_type>& visitor) override { visitor.Visit(*this); }

  nonterm_type GetOperandLhs() { return operand_lhs_; }
  term_type GetOperator() { return operator_; };
  nonterm_type GetOperandRhs() { return operand_rhs_; }

 private:
  nonterm_type operand_lhs_;
  term_type operator_;
  nonterm_type operand_rhs_;
};
template <template <class> class TMakeType, typename TTerm>
class AstProgram : public Ast<TMakeType, TTerm> {
 public:
  using nonterm_type = typename Ast<TMakeType, TTerm>::nonterm_type;
  using term_type = typename Ast<TMakeType, TTerm>::term_type;

  explicit AstProgram(nonterm_type program_id, nonterm_type program) : program_id_(program_id), program_(program) {}

  AstTypeId GetTypeId() override { return AstTypeId::kAstProgram; }

  void Accept(IAstVisitor<TMakeType, term_type>& visitor) override { visitor.Visit(*this); }

  nonterm_type GetProgramId() { return program_id_; }

  nonterm_type GetProgram() { return program_; }

 private:
  nonterm_type program_id_;
  nonterm_type program_;
};

template <template <class> class TMakeType, typename TTerm>
class AstVariableDeclaration : public Ast<TMakeType, TTerm> {
 public:
  using nonterm_type = typename Ast<TMakeType, TTerm>::nonterm_type;
  using term_type = typename Ast<TMakeType, TTerm>::term_type;

  explicit AstVariableDeclaration(term_type id, term_type type) : id_(id), type_(type) {}

  AstTypeId GetTypeId() override { return AstTypeId::kAstVariableDeclaration; }

  term_type GetId() { return id_; }
  term_type GetType() { return type_; }

  void Accept(IAstVisitor<TMakeType, term_type>& visitor) override { visitor.Visit(*this); }

 private:
  term_type id_;
  term_type type_;
};

template <template <class> class TMakeType, typename TTerm>
class AstBlock : public Ast<TMakeType, TTerm> {
 public:
  using nonterm_type = typename Ast<TMakeType, TTerm>::nonterm_type;
  using term_type = typename Ast<TMakeType, TTerm>::term_type;

  explicit AstBlock(std::vector<nonterm_type> const& var_decls, nonterm_type compound_statement)
      : var_decls_(var_decls), compound_statement_(compound_statement) {}

  AstTypeId GetTypeId() override { return AstTypeId::kAstRawListType; }

  std::vector<nonterm_type> GetVarDeclarations() { return var_decls_; }
  nonterm_type GetCompoundStatement() { return compound_statement_; }
  void Accept(IAstVisitor<TMakeType, term_type>& visitor) override { visitor.Visit(*this); }

 private:
  std::vector<nonterm_type> var_decls_;
  nonterm_type compound_statement_;
};

}  // namespace base

#endif