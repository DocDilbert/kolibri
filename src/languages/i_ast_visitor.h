#ifndef KOLIBRI_SRC_I_AST_VISITOR_H_
#define KOLIBRI_SRC_I_AST_VISITOR_H_

#include "languages/ast_id.h"

namespace languages {

struct VisitorReturnType {
};

template <template <class> class TMakeType, typename TTerm>
class AstNop;

template <template <class> class TMakeType, typename TTerm>
class AstId;

template <template <class> class TMakeType, typename TTerm>
class AstProgram;

template <template <class> class TMakeType, typename TTerm>
class AstBlock;

template <template <class> class TMakeType, typename TTerm>
class AstVariableDeclaration;

template <template <class> class TMakeType, typename TTerm>
class AstConst;

template <template <class> class TMakeType, typename TTerm>
class AstCompoundStatement;

template <template <class> class TMakeType, typename TTerm>
class AstUnaryOp;

template <template <class> class TMakeType, typename TTerm>
class AstBinaryOp;

template <template <class> class TMakeType, typename TTerm>
class IAstVisitor {
 public:
  using term_type = TTerm;

  virtual VisitorReturnType Visit(AstId<TMakeType, TTerm>& ast) = 0;
  virtual VisitorReturnType Visit(AstNop<TMakeType, TTerm>& ast) = 0;
  virtual VisitorReturnType Visit(AstProgram<TMakeType, TTerm>& ast) = 0;
  virtual VisitorReturnType Visit(AstBlock<TMakeType, TTerm>& ast) = 0;
  virtual VisitorReturnType Visit(AstVariableDeclaration<TMakeType, TTerm>& ast) = 0;
  virtual VisitorReturnType Visit(AstConst<TMakeType, TTerm>& ast) = 0;
  virtual VisitorReturnType Visit(AstCompoundStatement<TMakeType, TTerm>& ast) = 0;
  virtual VisitorReturnType Visit(AstUnaryOp<TMakeType, TTerm>& ast) = 0;
  virtual VisitorReturnType Visit(AstBinaryOp<TMakeType, TTerm>& ast) = 0;
};
}  // namespace base
#endif