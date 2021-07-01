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

using CalcLexerRules = lexer::LexerRules<       
    CFactory<CalcTokenId::UNKNOWN>,                 
    CFactory<CalcTokenId::ENDOFFILE>,        
    lexer::SequenceRule<lexer::SkipFactory, lexer::MatcherRangeByPredicate<lexer::IsChar<' '>>>,         
    lexer::SequenceRule<CFactory<CalcTokenId::COMMENT>, lexer::MatcherRangeByStartStopDelimiter<StringProviderOpenComment, StringProviderCloseComment>>,                                                
    lexer::SequenceRule<CFactory<CalcTokenId::NULLTERM>, lexer::MatcherPredicate<lexer::IsChar<'\000'>>>, 
    lexer::SequenceRule<CFactory<CalcTokenId::LPARENS>, lexer::MatcherPredicate<lexer::IsChar<'('>>>,
    lexer::SequenceRule<CFactory<CalcTokenId::RPARENS>, lexer::MatcherPredicate<lexer::IsChar<')'>>>, 
    lexer::SequenceRule<CFactory<CalcTokenId::PLUS>, lexer::MatcherPredicate<lexer::IsChar<'+'>>>,
    lexer::SequenceRule<CFactory<CalcTokenId::MINUS>, lexer::MatcherPredicate<lexer::IsChar<'-'>>>, 
    lexer::SequenceRule<CFactory<CalcTokenId::MULTIPLY>, lexer::MatcherPredicate<lexer::IsChar<'*'>>>,
    lexer::SequenceRule<CFactory<CalcTokenId::DIVIDE>, lexer::MatcherPredicate<lexer::IsChar<'/'>>>,
    lexer::SequenceRule<CFactory<CalcTokenId::INTEGER>, lexer::MatcherRangeByPredicate<lexer::IsDigit>>
>;

using CalcLexer = lexer::Lexer<CalcLexerRules>;

// clang-format on  

}
}
#endif
