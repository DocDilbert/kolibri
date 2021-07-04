#ifndef KOLIBRI_SRC_LEXER_TRAITS_H_
#define KOLIBRI_SRC_LEXER_TRAITS_H_

#include <type_traits>

namespace lexer {

class SkipProduction {
 public:
  using value_type = void;
};

template <class T>

struct is_skip_production_class : std::integral_constant<bool, std::is_base_of<SkipProduction, T>::value> {};

}  // namespace lexer

#endif