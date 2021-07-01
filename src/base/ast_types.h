#ifndef KOLIBRI_SRC_AST_TYPES_H_
#define KOLIBRI_SRC_AST_TYPES_H_

namespace base {
template <typename T>
class MakeShared {
 public:
  using type = std::shared_ptr<T>;
};
}  // namespace base

#endif