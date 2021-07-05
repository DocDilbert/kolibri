#ifndef KOLIBRI_SRC_I_AST_VISITOR_H_
#define KOLIBRI_SRC_I_AST_VISITOR_H_

#include "languages/ast_id.h"

namespace languages {

enum VisitorReturnType { kNone, kInteger, kDouble };

struct VisitorReturn {
  VisitorReturn() : return_type(kNone) {}
  VisitorReturn(int value) : return_type(kInteger), return_dbl(0.0f), return_int(value) {}
  VisitorReturn(double value) : return_type(kDouble), return_dbl(value), return_int(0) {}
  VisitorReturnType return_type;
  double return_dbl;
  int return_int;
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

  virtual VisitorReturn Visit(AstId<TMakeType, TTerm>& ast) = 0;
  virtual VisitorReturn Visit(AstNop<TMakeType, TTerm>& ast) = 0;
  virtual VisitorReturn Visit(AstProgram<TMakeType, TTerm>& ast) = 0;
  virtual VisitorReturn Visit(AstBlock<TMakeType, TTerm>& ast) = 0;
  virtual VisitorReturn Visit(AstVariableDeclaration<TMakeType, TTerm>& ast) = 0;
  virtual VisitorReturn Visit(AstConst<TMakeType, TTerm>& ast) = 0;
  virtual VisitorReturn Visit(AstCompoundStatement<TMakeType, TTerm>& ast) = 0;
  virtual VisitorReturn Visit(AstUnaryOp<TMakeType, TTerm>& ast) = 0;
  virtual VisitorReturn Visit(AstBinaryOp<TMakeType, TTerm>& ast) = 0;
};
}  // namespace languages
#endif