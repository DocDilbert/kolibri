#ifndef KOLIBRI_SRC_AST_ID_H_
#define KOLIBRI_SRC_AST_ID_H_

namespace base {

enum class ConstType {
  INTEGER,  //
  REAL
};

enum class UnaryOpType {
  POSITIVE_OP,  //
  NEGATIVE_OP
};

enum class BinaryOpType {
  ADD,  //
  SUB,
  MUL,
  INTEGER_DIV,
  FLOAT_DIV,
  ASSSIGN
};

}  // namespace base
#endif
