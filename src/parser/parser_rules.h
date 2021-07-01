#ifndef INTERTEST_SRC_PARSER_RULES_H_
#define INTERTEST_SRC_PARSER_RULES_H_
#include <assert.h>

#include <memory>
#include <vector>

#include "parser/i_parser_factory.h"
#include "parser/rule_id.h"

namespace parser {

template <typename TNonTerm>
struct RuleResult {
  using nonterm_type = TNonTerm;

  RuleResult(bool arg_is_match, nonterm_type arg_node, bool arg_error, const char* arg_msg)
      : is_match(arg_is_match), node(arg_node), is_error(arg_error), msg(arg_msg) {}
  bool is_match;
  nonterm_type node;
  bool is_error;
  const char* msg;
};

struct Result {
  Result(bool arg_is_match, bool arg_error, const char* arg_msg) : is_match(arg_is_match), is_error(arg_error), msg(arg_msg) {}
  bool is_match;
  bool is_error;
  const char* msg;
};
template <typename TNonTerm, typename Iterator>
class IParserGrammar {
 public:
  using nonterm_type = TNonTerm;
  using result_type = RuleResult<nonterm_type>;  // Use value type of first factory

  virtual result_type Match(IParserFactory<nonterm_type, typename Iterator::value_type>& parser_factory, RuleId rule_id, Iterator& it, Iterator end) = 0;
};

// ------------------------------------------------------------------------------------------------
// RULES
// ------------------------------------------------------------------------------------------------

// This expression doesn't consume anything but returns a match
class EmptyExpr {
 public:
  template <typename Production, typename TNonTerm, typename Iterator>
  Result Match(Production& production, IParserFactory<TNonTerm, typename Iterator::value_type>& parser_factory,
               IParserGrammar<TNonTerm, Iterator>& parser_grammar, Iterator& it, Iterator end) {
    auto result = Result(true, false, "");
    return result;
  };
};

// This expression consumes a terminal from the input iterator when the TokenPedicate matches
template <typename TermPredicate>
class TermExpr {
 public:
  template <typename Production, typename TNonTerm, typename Iterator>
  Result Match(Production& production, IParserFactory<TNonTerm, typename Iterator::value_type>& parser_factory,
               IParserGrammar<TNonTerm, Iterator>& parser_grammar, Iterator& it, Iterator end) {
    if (it == end) {
      auto result = Result(false, false, "TermExpr: No match");
      return result;
    }

    auto token = *it;  // peek
    TermPredicate predicate;

    if (!predicate(token)) {
      auto result = Result(false, false, "TermExpr: No match");
      return result;
    }

    it++;  // pop
    production.AddTerminal(token);
    auto result = Result(true, false, "");
    return result;
  };
};

// This expression calls another non terminal rule of the grammar
template <RuleId RId>
class NonTermExpr {
 public:
  template <typename Production, typename TNonTerm, typename Iterator>
  Result Match(Production& production, IParserFactory<TNonTerm, typename Iterator::value_type>& parser_factory,
               IParserGrammar<TNonTerm, Iterator>& parser_grammar, Iterator& it, Iterator end) {
    if (it == end) {
      auto result = Result(false, false, "NonTermExpr: No match");
      return result;
    }
    auto terminal_result = parser_grammar.Match(parser_factory, RId, it, end);
    if (terminal_result.is_error) {
      auto result = Result(false, true, terminal_result.msg);
      return result;
    }
    if (!terminal_result.is_match) {
      auto result = Result(false, false, "NonTermExpr: No match");
      return result;
    }
    production.AddNonTerminal(terminal_result.node);

    auto result = Result(true, false, "");
    return result;
  };
};

// e1 | e2 | ... | en
template <typename... Expressions>
class OrderedChoiceExpr {
 public:
  template <typename Production, typename TNonTerm, typename Iterator>
  Result Match(Production& production, IParserFactory<TNonTerm, typename Iterator::value_type>& parser_factory,
               IParserGrammar<TNonTerm, Iterator>& parser_grammar, Iterator& it, Iterator end) {
    if (it == end) {
      auto result = Result(false, false, "OrderedChoiceExpr: No match");
      return result;
    }

    auto result = MatchRecursive<0, Production, TNonTerm, Iterator, Expressions...>(production, parser_factory, parser_grammar, it, end);
    return result;
  };

 private:
  template <unsigned Idx, typename Production, typename TNonTerm, typename Iterator, typename T1>
  Result MatchRecursive(Production& production, IParserFactory<TNonTerm, typename Iterator::value_type>& parser_factory,
                        IParserGrammar<TNonTerm, Iterator>& parser_grammar, Iterator& it, Iterator end) {
    T1 expression;
    auto result = expression.Match(production, parser_factory, parser_grammar, it, end);
    if (result.is_match) {
      return result;
    }
    if (result.is_error) {
      return result;
    }
    result = Result(false, false, "OrderedChoiceExpr: No match");
    return result;
  }

  template <unsigned Idx, typename Production, typename TNonTerm, typename Iterator, typename T1, typename T2, typename... Args>
  Result MatchRecursive(Production& production, IParserFactory<TNonTerm, typename Iterator::value_type>& parser_factory,
                        IParserGrammar<TNonTerm, Iterator>& parser_grammar, Iterator& it, Iterator end) {
    T1 expression;
    auto result = expression.Match(production, parser_factory, parser_grammar, it, end);
    if (result.is_match) {
      return result;
    }
    if (result.is_error) {
      return result;
    }

    return MatchRecursive<Idx + 1, Production, TNonTerm, Iterator, T2, Args...>(production, parser_factory, parser_grammar, it, end);
  }
};

// e1?
template <typename Expression>
class OptionalExpr {
 public:
  template <typename Production, typename TNonTerm, typename Iterator>
  Result Match(Production& production, IParserFactory<TNonTerm, typename Iterator::value_type>& parser_factory,
               IParserGrammar<TNonTerm, Iterator>& parser_grammar, Iterator& it, Iterator end) {
    if (it == end) {
      auto result = Result(true, false, "");
      return result;
    }

    Expression expr;
    auto res = expr.Match(production, parser_factory, parser_grammar, it, end);
    if (!res.is_match) {
      return Result(true, false, "");
    }

    return res;
  };
};

// e1*  or e2+
template <unsigned N, typename Expression>
class NMatchesOrMoreExpr {
 public:
  template <typename Production, typename TNonTerm, typename Iterator>
  Result Match(Production& production, IParserFactory<TNonTerm, typename Iterator::value_type>& parser_factory,
               IParserGrammar<TNonTerm, Iterator>& parser_grammar, Iterator& it, Iterator end) {
    for (unsigned i = 0; i < N; ++i) {
      if (it == end) {
        return Result(false, false, "NMatchesOrMoreExpr: No match");
      }
      Expression expr;
      auto res = expr.Match(production, parser_factory, parser_grammar, it, end);
      if (!res.is_match) {
        return Result(false, false, "NMatchesOrMoreExpr: No match");
      }
      if (res.is_error) {
        return res;
      }
    }

    while (1) {
      if (it == end) {
        return Result(true, false, "");
      }

      Expression expr;
      auto res = expr.Match(production, parser_factory, parser_grammar, it, end);

      if (!res.is_match) {
        return Result(true, false, "");
      }
      if (res.is_error) {
        return res;
      }
    }
    return Result(false, true, "ERROR: Not reachable");
  };
};

// e1 e2 e3 e4
template <typename... Expressions>
class SequenceExpr {
 public:
  SequenceExpr() {}

  template <typename Production, typename TNonTerm, typename Iterator>
  Result Match(Production& production, IParserFactory<TNonTerm, typename Iterator::value_type>& parser_factory,
               IParserGrammar<TNonTerm, Iterator>& parser_grammar, Iterator& it, Iterator end) {
    auto backup_it = it;
    auto result = MatchRulesRecursive<Production, TNonTerm, Iterator, Expressions...>(production, parser_factory, parser_grammar, it, end);
    if (!result.is_match) {
      it = backup_it;
    }
    return result;
  };

 private:
  template <typename Production, typename TNonTerm, typename Iterator, typename T1>
  Result MatchRulesRecursive(Production& production, IParserFactory<TNonTerm, typename Iterator::value_type>& parser_factory,
                             IParserGrammar<TNonTerm, Iterator>& parser_grammar, Iterator& it, Iterator end) {
    T1 t1;
    auto res = t1.Match(production, parser_factory, parser_grammar, it, end);

    if (!res.is_match) {
      return Result(false, false, "SequenceExpr  -  No match");
    }
    if (res.is_error) {
      return res;
    }

    return Result(true, false, "");
  }

  template <typename Production, typename TNonTerm, typename Iterator, typename T1, typename T2, typename... Expr>
  Result MatchRulesRecursive(Production& production, IParserFactory<TNonTerm, typename Iterator::value_type>& parser_factory,
                             IParserGrammar<TNonTerm, Iterator>& parser_grammar, Iterator& it, Iterator end) {
    T1 t1;
    auto res = t1.Match(production, parser_factory, parser_grammar, it, end);

    if (!res.is_match) {
      return Result(false, false, "SequenceExpr  -  No match");
    }
    if (res.is_error) {
      return res;
    }

    auto result = MatchRulesRecursive<Production, TNonTerm, Iterator, T2, Expr...>(production, parser_factory, parser_grammar, it, end);
    return result;
  }
};

template <template <class, class> class Production, typename Expression>
class Rule {
 public:
  Rule(RuleId rule_id) : rule_id_(rule_id) {}

  template <typename TNonTerm, typename Iterator>
  RuleResult<TNonTerm> Match(IParserFactory<TNonTerm, typename Iterator::value_type>& parser_factory, IParserGrammar<TNonTerm, Iterator>& parser_grammar,
                             Iterator& it, Iterator end) {
    auto backup_it = it;

    if (it == end) {
      return RuleResult<TNonTerm>(false, parser_factory.CreateNull(), false, "Rule -  No match");
    }

    Production<TNonTerm, typename Iterator::value_type> production(rule_id_);
    Expression expr;
    auto res = expr.Match(production, parser_factory, parser_grammar, it, end);

    if (!res.is_match) {
      it = backup_it;
      return RuleResult<TNonTerm>(false, parser_factory.CreateNull(), false, "Rule -  No match");
    }
    if (res.is_error) {
      return RuleResult<TNonTerm>(false, parser_factory.CreateNull(), true, res.msg);
    }

    // perform production
    auto node = production.Create(parser_factory);
    return RuleResult<TNonTerm>(true, node, false, "");
  };

 private:
  RuleId rule_id_;
};

template <typename... Args>
class OrderedChoiceRules {
 public:
  OrderedChoiceRules(RuleId rule_id) : rule_id_(rule_id) {}

  template <typename TNonTerm, typename Iterator>
  RuleResult<TNonTerm> Match(IParserFactory<TNonTerm, typename Iterator::value_type>& parser_factory, IParserGrammar<TNonTerm, Iterator>& parser_grammar,
                             Iterator& it, Iterator end) {
    if (it == end) {
      auto result = RuleResult<TNonTerm>(false, parser_factory.CreateNull(), true, "ERROR: Unexpected END");
      return result;
    }
    return MatchRulesRecursive<TNonTerm, Iterator, Args...>(parser_factory, parser_grammar, it, end);
  };

 private:
  template <typename TNonTerm, typename Iterator, typename T1>
  RuleResult<TNonTerm> MatchRulesRecursive(IParserFactory<TNonTerm, typename Iterator::value_type>& parser_factory,
                                           IParserGrammar<TNonTerm, Iterator>& parser_grammar, Iterator& it, Iterator end) {
    T1 t1(rule_id_);
    auto res = t1.Match(parser_factory, parser_grammar, it, end);
    if (res.is_match) {
      return res;
    }
    if (res.is_error) {
      return res;
    }
    return RuleResult<TNonTerm>(false, parser_factory.CreateNull(), false, "OrderedChoiceRule -  No match");
  }

  template <typename TNonTerm, typename Iterator, typename T1, typename T2, typename... Rules>
  RuleResult<TNonTerm> MatchRulesRecursive(IParserFactory<TNonTerm, typename Iterator::value_type>& parser_factory,
                                           IParserGrammar<TNonTerm, Iterator>& parser_grammar, Iterator& it, Iterator end) {
    T1 t1(rule_id_);

    auto res = t1.Match(parser_factory, parser_grammar, it, end);
    if (res.is_match) {
      return res;
    }
    if (res.is_error) {
      return res;
    }
    return MatchRulesRecursive<TNonTerm, Iterator, T2, Rules...>(parser_factory, parser_grammar, it, end);
  }

 private:
  RuleId rule_id_;
};

// ------------------------------------------------------------------------------------------------
// GRAMMAR
// ------------------------------------------------------------------------------------------------

template <typename TNonTerm, typename Iterator, typename... Terminals>
class ParserGrammar : public IParserGrammar<TNonTerm, Iterator> {
 public:
  using nonterm_type = TNonTerm;
  using iterator_type = Iterator;
  using term_type = typename iterator_type::value_type;
  using result_type = RuleResult<nonterm_type>;  // Use value type of first factory

  ParserGrammar() {}
  result_type Match(IParserFactory<nonterm_type, term_type>& parser_factory, RuleId rule_id, iterator_type& it, iterator_type end) override {
    if (it == end) {
      auto result = result_type(false, parser_factory.CreateNull(), true, "ERROR: Unexpected END");
      return result;
    }

    return CallRule<0, Terminals...>(parser_factory, rule_id, it, end);
  };

  template <unsigned Idx, typename T1>
  result_type CallRule(IParserFactory<nonterm_type, term_type>& parser_factory, RuleId rule_id, iterator_type& it, iterator_type end) {
    assert(static_cast<unsigned>(rule_id) == Idx);
    T1 t1(rule_id);

    return t1.Match(parser_factory, *this, it, end);  // assume last rule does match every time
  }

  template <unsigned Idx, typename T1, typename T2, typename... Rest>
  result_type CallRule(IParserFactory<nonterm_type, term_type>& parser_factory, RuleId rule_id, iterator_type& it, iterator_type end) {
    if (static_cast<unsigned>(rule_id) == Idx) {
      T1 t1(rule_id);
      return t1.Match(parser_factory, *this, it, end);
    }
    return CallRule<Idx + 1, T2, Rest...>(parser_factory, rule_id, it, end);
  }
};

struct GrammarBase {
  template <template <class, class> class Production, typename Expression>
  using Rule = parser::Rule<Production, Expression>;

  using EmptyExpr = parser::EmptyExpr;

  template <typename TermPredicate>
  using TermExpr = parser::TermExpr<TermPredicate>;

  template <RuleId RId>
  using NonTermExpr = parser::NonTermExpr<RId>;

  template <typename... Expressions>
  using SequenceExpr = parser::SequenceExpr<Expressions...>;

  template <unsigned N, typename Expression>
  using NMatchesOrMoreExpr = parser::NMatchesOrMoreExpr<N, Expression>;

  template <typename... Expressions>
  using OrderedChoiceExpr = parser::OrderedChoiceExpr<Expressions...>;

  template <typename... Args>
  using OrderedChoiceRules = parser::OrderedChoiceRules<Args...>;
};

}  // namespace parser

#endif