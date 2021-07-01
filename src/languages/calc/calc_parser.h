#ifndef INTERTEST_SRC_CALC_PARSER_H_
#define INTERTEST_SRC_CALC_PARSER_H_

#include "base/ast.h"
#include "base/ast_types.h"
#include "base/i_ast_factory.h"
#include "base/token.h"
#include "languages/calc/calc_lexer.h"
#include "languages/calc/calc_token.h"
#include "parser/i_parser_factory.h"
#include "parser/parser.h"
#include "parser/parser_productions.h"
#include "parser/parser_rules.h"

namespace languages {
namespace calc {

template <CalcTokenId Id>
class CalcTokenPredicate {
 public:
  bool operator()(CalcToken token) { return token.GetId() == Id; };
};

template <typename TNonTerm, typename Iterator>
struct CalculatorGrammar : public parser::GrammarBase {
  // clang-format off
  using type = parser::ParserGrammar<
    TNonTerm, Iterator,

    // Rule #0
    Rule<parser::NonTermTermNonTermSequenceProduction,
      SequenceExpr<
        NonTermExpr<parser::RuleId::kRule1>,
        NMatchesOrMoreExpr<0,
          SequenceExpr<
            OrderedChoiceExpr<
              TermExpr<CalcTokenPredicate<CalcTokenId::kPlus>>, 
              TermExpr<CalcTokenPredicate<CalcTokenId::kMinus>>
            >, 
            NonTermExpr<parser::RuleId::kRule1>
          >
        >
      >
    >,
    

    // Rule #1
    Rule<parser::NonTermTermNonTermSequenceProduction, 
      SequenceExpr<
        NonTermExpr<parser::RuleId::kRule2>,
        NMatchesOrMoreExpr<0,
          SequenceExpr<
            OrderedChoiceExpr<
              TermExpr<CalcTokenPredicate<CalcTokenId::kMultiply>>,
              TermExpr<CalcTokenPredicate<CalcTokenId::kDivide>>
            >, 
            NonTermExpr<parser::RuleId::kRule2>
          >
        >
      >
    >,

    // Rule #2
    OrderedChoiceRules< 
        Rule<parser::TermNonTermProduction, 
          SequenceExpr<
            TermExpr<CalcTokenPredicate<CalcTokenId::kPlus>>, 
            NonTermExpr<parser::RuleId::kRule2>
          >
        >,
        Rule<parser::TermNonTermProduction, 
          SequenceExpr<
            TermExpr<CalcTokenPredicate<CalcTokenId::kMinus>>, 
            NonTermExpr<parser::RuleId::kRule2>
          >
        >,
        Rule<parser::BypassLastTermProduction, 
          SequenceExpr<
            TermExpr<CalcTokenPredicate<CalcTokenId::kLParens>>, 
            NonTermExpr<parser::RuleId::kRule0>, 
            TermExpr<CalcTokenPredicate<CalcTokenId::kRParens>>
          >
        >,
        Rule<parser::TermProduction, 
          SequenceExpr<TermExpr<CalcTokenPredicate<CalcTokenId::kInteger>>>
        >
    >
  >;
  // clang-format on
};

using CalcGrammar = CalculatorGrammar<std::shared_ptr<base::Ast<base::MakeShared, CalcToken>>, CalcLexer::iterator_type>::type;
using CalcParser = parser::Parser<CalcGrammar>;

}  // namespace calc
}  // namespace languages
#endif