#ifndef INTERTEST_SRC_LEXER_RULES_H_
#define INTERTEST_SRC_LEXER_RULES_H_

#include "lexer/lexer_transforms.h"

namespace lexer {

template <typename StringProvider, bool CaseInsensitive = false>
class MatcherString {
 public:
  const char* Parse(const char* begin, const char* end) {
    const char* cmp = StringProvider().GetString();
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

template <typename StringProvider>
class MatcherString<StringProvider, true> {
 public:
  const char* Parse(const char* begin, const char* end) {
    const char* cmp = StringProvider().GetString();
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
    return pos;
  }
};

template <typename Predicate>
class MatcherPredicate {
 public:
  const char* Parse(const char* begin, const char* end) {
    Predicate pred;
    if (begin == end) {
      return begin;
    }
    if (pred(*begin)) {
      return begin + 1;
    }
    return begin;
  }
};

template <typename Predicate>
class MatcherRangeByPredicate {
 public:
  const char* Parse(const char* begin, const char* end) {
    Predicate pred;
    const char* pos = begin;
    for (; pos < end; pos++) {
      if (!pred(*pos)) {
        break;
      }
    }
    return pos;
  }
};

template <typename StartStringProvider, typename StopStringProvider>
class MatcherRangeByStartStopDelimiter {
 public:
  const char* Parse(const char* begin, const char* end) {
    MatcherString<StartStringProvider> start_matcher;
    auto it_ = start_matcher.Parse(begin, end);
    if (it_ == begin) {
      return begin;
    }

    const char* pos = it_;
    MatcherString<StopStringProvider> stop_matcher;
    for (; pos < end; pos++) {
      it_ = stop_matcher.Parse(pos, end);
      if (it_ != pos) {
        return it_;
      }
    }
    return begin;
  }
};

template <typename Factory, typename... Args>
class SequenceRule {
 public:
  using value_type = typename Factory::value_type;
  const char* Match(const char* begin, const char* end) { return MatchRecursive<Args...>(begin, begin, end); }

  value_type Create(const char* begin, const char* end) {
    Factory factory;
    return factory.Create(begin, end);
  }

 private:
  template <typename T1>
  const char* MatchRecursive(const char* begin, const char* it, const char* end) {
    T1 matcher;
    auto* new_it = matcher.Parse(it, end);
    if (new_it == it) {
      return begin;
    }
    return new_it;
  }

  template <typename T1, typename T2, typename... Matchers>
  const char* MatchRecursive(const char* begin, const char* it, const char* end) {
    T1 matcher;
    auto* new_it = matcher.Parse(it, end);
    if (new_it == it) {
      return begin;
    }
    if (new_it == end) {
      return begin;
    }
    return MatchRecursive<T2, Matchers...>(begin, new_it, end);
  }
};

template <typename FactoryUNK, typename FactoryEOF, typename... Args>
class LexerRules {
 public:
  using value_type = typename FactoryUNK::value_type;  // Use value type of first factory

  value_type Match(const char* begin, const char* end) {
    // reached end
    if (begin == end) {
      it_ = end;
      return FactoryEOF().Create(begin, end);
    }

    return MatchRecursive<Args...>(begin, end);
  };

  const char* GetPosition() { return it_; }

 private:
  const char* it_;
  template <typename T1>
  value_type MatchRecursive(const char* begin, const char* end) {
    T1 t1;
    auto it = t1.Match(begin, end);
    if (it != begin) {
      it_ = it;
      return t1.Create(begin, it_);
    }
    it_ = begin + 1;
    return FactoryUNK().Create(begin, it_);
  }

  template <typename T1, typename T2, typename... Rules>
  value_type MatchRecursive(const char* begin, const char* end) {
    T1 t1;
    auto it = t1.Match(begin, end);

    if (it != begin) {
      it_ = it;
      return t1.Create(begin, it_);
    }

    return MatchRecursive<T2, Rules...>(begin, end);
  }
};

}  // namespace lexer
#endif
