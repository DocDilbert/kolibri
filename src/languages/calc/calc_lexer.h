#ifndef INTERTEST_SRC_CALC_LEXER_H_
#define INTERTEST_SRC_CALC_LEXER_H_

#include "languages/calc/calc_token.h"
#include "lexer/lexer.h"
#include "lexer/lexer_predicates.h"
#include "lexer/lexer_rules.h"
#include "string_providers.h"

namespace languages {
namespace calc {

template <CalcTokenId Id>
class CFactory {
 public:
  using value_type = CalcToken;
  value_type Create(const char* begin, const char* end) { return CalcToken(Id, begin, end); }
};

// clang-format off
struct CalcLexerRules {
  template <typename Factory, typename... Args>
  using SequenceRule = lexer::SequenceRule<Factory, Args...>;

  using type = lexer::LexerRules<       
      CFactory<CalcTokenId::kUnknown>,                 
      CFactory<CalcTokenId::kEndOfFile>,        
      // Skip rules
      SequenceRule<lexer::SkipFactory, lexer::MatcherRangeByPredicate<lexer::IsChar<' '>>>,         
      
      // Production rules
      SequenceRule<CFactory<CalcTokenId::kNullterm>, lexer::MatcherPredicate<lexer::IsChar<'\000'>>>, 
      SequenceRule<CFactory<CalcTokenId::kLParens>, lexer::MatcherPredicate<lexer::IsChar<'('>>>,
      SequenceRule<CFactory<CalcTokenId::kRParens>, lexer::MatcherPredicate<lexer::IsChar<')'>>>, 
      SequenceRule<CFactory<CalcTokenId::kPlus>, lexer::MatcherPredicate<lexer::IsChar<'+'>>>,
      SequenceRule<CFactory<CalcTokenId::kMinus>, lexer::MatcherPredicate<lexer::IsChar<'-'>>>, 
      SequenceRule<CFactory<CalcTokenId::kMultiply>, lexer::MatcherPredicate<lexer::IsChar<'*'>>>,
      SequenceRule<CFactory<CalcTokenId::kDivide>, lexer::MatcherPredicate<lexer::IsChar<'/'>>>,
      SequenceRule<CFactory<CalcTokenId::kInteger>, lexer::MatcherRangeByPredicate<lexer::IsDigit>>
  >;
};

using CalcLexer = lexer::Lexer<CalcLexerRules::type>;

// clang-format on  

}
}
#endif
