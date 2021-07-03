#ifndef KOLIBRI_SRC_CALC_LEXER_H_
#define KOLIBRI_SRC_CALC_LEXER_H_

#include "languages/calc/calc_token.h"
#include "lexer/lexer.h"
#include "lexer/lexer_predicates.h"
#include "lexer/lexer_rules.h"
#include "string_providers.h"

namespace languages {
namespace calc {

template <CalcTokenId Id>
class CalcLexerProduction {
 public:
  using value_type = CalcToken;
  value_type Create(const char* begin, const char* end) { return CalcToken(Id, begin, end); }
};

// clang-format off
struct CalcLexerRules : public lexer::LexerRulesBase {
  using type = lexer::LexerRules<       
      CalcLexerProduction<CalcTokenId::kUnknown>,                 
      CalcLexerProduction<CalcTokenId::kEndOfFile>, 
             
      // Skip rules
      Rule<SkipProduction, MatcherRangeByPredicate<lexer::IsChar<' '>>>,         
      
      // Production rules
      Rule<CalcLexerProduction<CalcTokenId::kNullterm>, MatcherPredicate<lexer::IsChar<'\000'>>>, 
      Rule<CalcLexerProduction<CalcTokenId::kLParens>, MatcherPredicate<lexer::IsChar<'('>>>,
      Rule<CalcLexerProduction<CalcTokenId::kRParens>, MatcherPredicate<lexer::IsChar<')'>>>, 
      Rule<CalcLexerProduction<CalcTokenId::kPlus>, MatcherPredicate<lexer::IsChar<'+'>>>,
      Rule<CalcLexerProduction<CalcTokenId::kMinus>, MatcherPredicate<lexer::IsChar<'-'>>>, 
      Rule<CalcLexerProduction<CalcTokenId::kMultiply>, MatcherPredicate<lexer::IsChar<'*'>>>,
      Rule<CalcLexerProduction<CalcTokenId::kDiv>, MatcherPredicate<lexer::IsChar<'/'>>>,
      Rule<CalcLexerProduction<CalcTokenId::kInteger>, MatcherRangeByPredicate<lexer::IsDigit>>
  >;
};

using CalcLexer = lexer::Lexer<CalcLexerRules::type>;

// clang-format on  

}
}
#endif
