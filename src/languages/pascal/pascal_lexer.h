#ifndef KOLIBRI_SRC_PASCAL_LEXER_H_
#define KOLIBRI_SRC_PASCAL_LEXER_H_

#include "languages/pascal/pascal_token.h"
#include "lexer/lexer.h"
#include "lexer/lexer_predicates.h"
#include "lexer/lexer_rules.h"
#include "string_providers.h"

namespace languages {
namespace pascal {

template <PascalTokenId id>
class PascalLexerProduction {
 public:
  using value_type = PascalToken;
  value_type Create(const char* begin, const char* end) { return PascalToken(id, begin, end); }
};

// clang-format off
struct PascalLexerRules : public lexer::LexerRulesBase {

  using type = lexer::LexerRules< 
      PascalLexerProduction<PascalTokenId::kUnknown>,                                                         
      PascalLexerProduction<PascalTokenId::kEndOfFile>, 

      // Skip rules
      Rule<SkipProduction, MatcherRangeByPredicate<lexer::IsChar<' '>>>,
      Rule<SkipProduction, MatcherRangeByPredicate<lexer::IsChar<'\n'>>>,
      Rule<SkipProduction, 
        lexer::MatcherRangeByStartStopDelimiter<
          MatcherString<StringProviderCurlyBraceOpen>,
          MatcherString<StringProviderCurlyBraceClose>
        >
      >,

      // Production rules
      Rule<PascalLexerProduction<PascalTokenId::kLParens>, MatcherPredicate<lexer::IsChar<'('>>>, 
      Rule<PascalLexerProduction<PascalTokenId::kRParens>, MatcherPredicate<lexer::IsChar<')'>>>,
      Rule<PascalLexerProduction<PascalTokenId::kPlus>, MatcherPredicate<lexer::IsChar<'+'>>>,                     
      Rule<PascalLexerProduction<PascalTokenId::kMinus>, MatcherPredicate<lexer::IsChar<'-'>>>,                    
      Rule<PascalLexerProduction<PascalTokenId::kMultiply>, MatcherPredicate<lexer::IsChar<'*'>>>,  
      Rule<PascalLexerProduction<PascalTokenId::kSemi>, MatcherPredicate<lexer::IsChar<';'>>>, 
      Rule<PascalLexerProduction<PascalTokenId::kDot>, MatcherPredicate<lexer::IsChar<'.'>>>,               
      Rule<PascalLexerProduction<PascalTokenId::kFloatDiv>, MatcherPredicate<lexer::IsChar<'/'>>>,
      Rule<PascalLexerProduction<PascalTokenId::kAssign>, 
        MatcherSequence<
          MatcherPredicate<lexer::IsChar<':'>>, 
          MatcherPredicate<lexer::IsChar<'='>>
        >
      >,
      Rule<PascalLexerProduction<PascalTokenId::kColon>, MatcherPredicate<lexer::IsChar<':'>>>,
      Rule<PascalLexerProduction<PascalTokenId::kComma>, MatcherPredicate<lexer::IsChar<','>>>,
      Rule<PascalLexerProduction<PascalTokenId::kIntegerDiv>, MatcherString<StringProviderDiv, true>>,  
      Rule<PascalLexerProduction<PascalTokenId::kProgram>, MatcherString<StringProviderProgram, true>>,
      Rule<PascalLexerProduction<PascalTokenId::kInteger>, MatcherString<StringProviderInteger, true>>,
      Rule<PascalLexerProduction<PascalTokenId::kReal>, MatcherString<StringProviderReal, true>>,
      Rule<PascalLexerProduction<PascalTokenId::kVar>, MatcherString<StringProviderVar, true>>, 
      Rule<PascalLexerProduction<PascalTokenId::kBegin>, MatcherString<StringProviderBegin, true>>,    
      Rule<PascalLexerProduction<PascalTokenId::kEnd>, MatcherString<StringProviderEnd, true>>,
      Rule<PascalLexerProduction<PascalTokenId::kRealConst>,
        MatcherSequence< 
          MatcherRangeByPredicate<lexer::IsDigit>, 
          MatcherPredicate<lexer::IsChar<'.'>>, 
          MatcherRangeByPredicate<lexer::IsDigit>
        >
      >,                 
      Rule<PascalLexerProduction<PascalTokenId::kIntegerConst>, MatcherRangeByPredicate<lexer::IsDigit>>, 
      Rule<PascalLexerProduction<PascalTokenId::kId>,
      MatcherSequence<  
          MatcherPredicate<
            lexer::PredicateOr<
              lexer::IsLetter, 
              lexer::IsChar<'_'>
            >
          >, 
          MatcherRangeByPredicate<lexer::IsLetterOrDigit>
        >
      >,
      Rule<PascalLexerProduction<PascalTokenId::kId>, 
        MatcherPredicate<
          lexer::PredicateOr<
            lexer::IsLetter, 
            lexer::IsChar<'_'>
          >
        >
      >
  >;
};

// clang-format on
using PascalLexer = lexer::Lexer<PascalLexerRules::type>;

}  // namespace pascal
}  // namespace languages

#endif