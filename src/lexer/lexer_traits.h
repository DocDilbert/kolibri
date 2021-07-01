#ifndef INTERTEST_SRC_LEXER_TRAITS_H_
#define INTERTEST_SRC_LEXER_TRAITS_H_

#include <type_traits>

namespace lexer {

class SkipFactory {};

template <class T>

struct is_skip_factory : std::integral_constant<bool, std::is_base_of<SkipFactory, T>::value> {};

}  // namespace lexer

#endif