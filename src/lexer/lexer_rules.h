#ifndef KOLIBRI_SRC_LEXER_RULES_H_
#define KOLIBRI_SRC_LEXER_RULES_H_

#include <type_traits>

#include "lexer/lexer_traits.h"
#include "lexer/lexer_transforms.h"

namespace lexer {

template <typename TStringProvider, bool CaseInsensitive = false>
class MatcherString {
 public:
  const char* Parse(const char* begin, const char* end) {
    const char* cmp = TStringProvider::GetString();
    const char* pos = begin;
    for (; pos < end; pos++) {
      if (*cmp == '\0') {
        break;
      }
      if (*cmp != *pos) {
        return begin;
      }
      cmp++;
    }

    // special case ... given input string is not long enough
    if (*cmp != '\0') {
      return begin;
    }

    return pos;
  }
};

template <typename TStringProvider>
class MatcherString<TStringProvider, true> {
 public:
  const char* Parse(const char* begin, const char* end) {
    const char* cmp = TStringProvider::GetString();
    const char* pos = begin;
    for (; pos < end; pos++) {
      if (*cmp == '\0') {
        break;
      }
      ToLowerCase to_lower_case;
      if (to_lower_case(*cmp) != to_lower_case(*pos)) {
        return begin;
      }
      cmp++;
    }

    // special case ... given input string is not long enough
    if (*cmp != '\0') {
      return begin;
    }
    return pos;
  }
};

template <typename TPredicate>
class MatcherPredicate {
 public:
  const char* Parse(const char* begin, const char* end) {
    TPredicate pred;
    if (begin == end) {
      return begin;
    }
    if (pred(*begin)) {
      return begin + 1;
    }
    return begin;
  }
};

template <typename TPredicate>
class MatcherRangeByPredicate {
 public:
  const char* Parse(const char* begin, const char* end) {
    TPredicate pred;
    const char* pos = begin;
    for (; pos < end; pos++) {
      if (!pred(*pos)) {
        break;
      }
    }
    return pos;
  }
};

template <typename TStartMatcher, typename TStopMatcher>
class MatcherRangeByStartStopDelimiter {
 public:
  const char* Parse(const char* begin, const char* end) {
    TStartMatcher start_matcher;
    auto it_ = start_matcher.Parse(begin, end);
    if (it_ == begin) {
      return begin;
    }

    const char* pos = it_;
    TStopMatcher stop_matcher;
    for (; pos < end; pos++) {
      it_ = stop_matcher.Parse(pos, end);
      if (it_ != pos) {
        return it_;
      }
    }
    return begin;
  }
};

template <typename... TMatchers>
class MatcherSequence {
 public:
  const char* Parse(const char* begin, const char* end) { return ParseRecursive<TMatchers...>(begin, begin, end); }

 private:
  template <typename T1>
  const char* ParseRecursive(const char* begin, const char* it, const char* end) {
    T1 matcher;
    auto* new_it = matcher.Parse(it, end);
    if (new_it == it) {
      return begin;
    }
    return new_it;
  }

  template <typename T1, typename T2, typename... UMatchers>
  const char* ParseRecursive(const char* begin, const char* it, const char* end) {
    T1 matcher;
    auto* new_it = matcher.Parse(it, end);
    if (new_it == it) {
      return begin;
    }
    if (new_it == end) {
      return begin;
    }
    return ParseRecursive<T2, UMatchers...>(begin, new_it, end);
  }
};

template <typename TProduction, typename TMatcher>
class Rule {
 public:
  using value_type = typename TProduction::value_type;
  using production_type = TProduction;

  const char* Match(const char* begin, const char* end) {
    TMatcher matcher;
    auto* it = matcher.Parse(begin, end);
    return it;
  }

  value_type Create(const char* begin, const char* end) {
    production_type factory;
    return factory.Create(begin, end);
  }
};

template <typename TProductionUNK, typename TProductionEOF, typename... TRules>
class LexerRules {
 public:
  using value_type = typename TProductionUNK::value_type;  // Use value type of first factory

  value_type Match(const char* begin, const char* end) {
    // reached end
    if (begin == end) {
      it_ = end;
      return TProductionEOF().Create(begin, end);
    }

    return MatchRecursive<TRules...>(begin, end);
  };

  const char* GetPosition() { return it_; }

 private:
  const char* it_;

  template <bool Cond>
  struct SelectCreateOrRedirect {
    template <typename TLexer, typename TRule>
    static constexpr value_type impl(TLexer& lexer, TRule& rule, const char* token_begin, const char* token_end, const char* end) noexcept {
      return rule.Create(token_begin, token_end);
    }
  };

  template <>
  struct SelectCreateOrRedirect<true> {
    template <typename TLexer, typename TRule>
    static constexpr value_type impl(TLexer& lexer, TRule& rule, const char* token_begin, const char* token_end, const char* end) noexcept {
      return lexer.Match(token_end, end);
    }
  };

  template <typename T1>
  value_type MatchRecursive(const char* begin, const char* end) {
    T1 t1;
    auto it = t1.Match(begin, end);
    if (it != begin) {
      it_ = it;
      return SelectCreateOrRedirect<is_skip_production_class<typename T1::production_type>::value>::impl(*this, t1, begin, it_, end);
    }
    it_ = begin + 1;
    return TProductionUNK().Create(begin, it_);
  }

  template <typename T1, typename T2, typename... URules>
  value_type MatchRecursive(const char* begin, const char* end) {
    T1 t1;
    auto it = t1.Match(begin, end);

    if (it != begin) {
      it_ = it;
      return SelectCreateOrRedirect<is_skip_production_class<typename T1::production_type>::value>::impl(*this, t1, begin, it_, end);
    }

    return MatchRecursive<T2, URules...>(begin, end);
  }
};

class LexerRulesBase {
 public:
  template <typename Factory, typename... Args>
  using Rule = lexer::Rule<Factory, Args...>;

  using SkipProduction = lexer::SkipProduction;

  template <typename TPredicate>
  using MatcherPredicate = lexer::MatcherPredicate<TPredicate>;

  template <typename TStringProvider, bool CaseInsensitive = false>
  using MatcherString = lexer::MatcherString<TStringProvider, CaseInsensitive>;

  template <typename TPredicate>
  using MatcherRangeByPredicate = lexer::MatcherRangeByPredicate<TPredicate>;

  template <typename... TMatchers>
  using MatcherSequence = lexer::MatcherSequence<TMatchers...>;
};

}  // namespace lexer
#endif
