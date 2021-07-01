#ifndef KOLIBRI_SRC_LEXER_PREDICATES_H_
#define KOLIBRI_SRC_LEXER_PREDICATES_H_

namespace lexer {

template <char Char>
struct IsChar {
  bool operator()(char ch) { return (ch == Char); }
};

struct IsDigit {
  bool operator()(char ch) { return (('0' <= ch) && (ch <= '9')); }
};

struct IsLowerCaseLetter {
  bool operator()(char ch) { return (('a' <= ch) && (ch <= 'z')); }
};

struct IsUpperCaseLetter {
  bool operator()(char ch) { return (('A' <= ch) && (ch <= 'Z')); }
};

struct IsLetter {
  bool operator()(char ch) {
    IsLowerCaseLetter is_alpha_lowercase;
    if (is_alpha_lowercase(ch)) {
      return true;
    }
    IsUpperCaseLetter is_alpha_uppercase;
    return (is_alpha_uppercase(ch));
  }
};

struct IsLetterOrDigit {
  bool operator()(char ch) {
    IsLetter is_alpha;
    if (is_alpha(ch)) {
      return true;
    }
    IsDigit is_digit;
    return (is_digit(ch));
  }
};

template <typename... Predicates>
struct PredicateOr {
  bool operator()(char ch) { return Recurse<Predicates...>(ch);}

  template <typename T1>
  bool Recurse(char ch) {
    T1 t1;
    return t1(ch);
  }
  template <typename T1, typename T2, typename... Args>
  bool Recurse(char ch) {
    T1 t1;
    return t1(ch) || Recurse<T2, Args...>(ch);
  }
};

}  // namespace lexer

#endif
