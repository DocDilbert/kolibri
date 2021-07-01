#ifndef KOLIBRI_SRC_AST_ID_H_
#define KOLIBRI_SRC_AST_ID_H_

namespace base {

enum class ConstType {
  kInteger,  //
  kReal
};

enum class UnaryOpType {
  kPositiveOp,  //
  kNegativeOp
};

enum class BinaryOpType {
  kAdd,  //
  kSub,
  kMul,
  kIntegerDiv,
  kFloatDiv,
  kAssign
};

}  // namespace base
#endif
