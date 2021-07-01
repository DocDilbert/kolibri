#ifndef INTERTEST_SRC_PASCAL_LEXER_H_
#define INTERTEST_SRC_PASCAL_LEXER_H_

#include "languages/pascal/pascal_token.h"
#include "lexer/lexer_predicates.h"
#include "lexer/lexer_rules.h"
#include "string_providers.h"

namespace languages {
namespace pascal {

template <PascalTokenId id>
class PFactory {
 public:
  using value_type = PascalToken;
  value_type Create(const char* begin, const char* end) { return PascalToken(id, begin, end); }
};

// clang-format off
struct PascalLexerRules {
  template <typename Factory, typename... Args>
  using SequenceRule = lexer::SequenceRule<Factory, Args...>;

  using type = lexer::LexerRules< 
      PFactory<PascalTokenId::kUnknown>,                                                         
      PFactory<PascalTokenId::kEndOfFile>, 

      // Skip rules
      SequenceRule<lexer::SkipFactory, lexer::MatcherRangeByPredicate<lexer::IsChar<' '>>>,
      SequenceRule<lexer::SkipFactory, lexer::MatcherRangeByPredicate<lexer::IsChar<'\n'>>>,
      SequenceRule<lexer::SkipFactory, 
        lexer::MatcherRangeByStartStopDelimiter<
          StringProviderCurlyBraceOpen,
          StringProviderCurlyBraceClose
        >
      >,

      // Production rules
      SequenceRule<PFactory<PascalTokenId::kLParens>, lexer::MatcherPredicate<lexer::IsChar<'('>>>, 
      SequenceRule<PFactory<PascalTokenId::kRParens>, lexer::MatcherPredicate<lexer::IsChar<')'>>>,
      SequenceRule<PFactory<PascalTokenId::kPlus>, lexer::MatcherPredicate<lexer::IsChar<'+'>>>,                     
      SequenceRule<PFactory<PascalTokenId::kMinus>, lexer::MatcherPredicate<lexer::IsChar<'-'>>>,                    
      SequenceRule<PFactory<PascalTokenId::kMultiply>, lexer::MatcherPredicate<lexer::IsChar<'*'>>>,  
      SequenceRule<PFactory<PascalTokenId::kSemi>, lexer::MatcherPredicate<lexer::IsChar<';'>>>, 
      SequenceRule<PFactory<PascalTokenId::kDot>, lexer::MatcherPredicate<lexer::IsChar<'.'>>>,               
      SequenceRule<PFactory<PascalTokenId::kFloatDiv>, lexer::MatcherPredicate<lexer::IsChar<'/'>>>,
      SequenceRule<PFactory<PascalTokenId::kAssign>, 
        lexer::MatcherPredicate<lexer::IsChar<':'>>, 
        lexer::MatcherPredicate<lexer::IsChar<'='>>
      >,
      SequenceRule<PFactory<PascalTokenId::kColon>, lexer::MatcherPredicate<lexer::IsChar<':'>>>,
      SequenceRule<PFactory<PascalTokenId::kComma>, lexer::MatcherPredicate<lexer::IsChar<','>>>,
      SequenceRule<PFactory<PascalTokenId::kIntegerDiv>, lexer::MatcherString<StringProviderDiv, true>>,  
      SequenceRule<PFactory<PascalTokenId::kProgram>, lexer::MatcherString<StringProviderProgram, true>>,
      SequenceRule<PFactory<PascalTokenId::kInteger>, lexer::MatcherString<StringProviderInteger, true>>,
      SequenceRule<PFactory<PascalTokenId::kReal>, lexer::MatcherString<StringProviderReal, true>>,
      SequenceRule<PFactory<PascalTokenId::kVar>, lexer::MatcherString<StringProviderVar, true>>, 
      SequenceRule<PFactory<PascalTokenId::kBegin>, lexer::MatcherString<StringProviderBegin, true>>,    
      SequenceRule<PFactory<PascalTokenId::kEnd>, lexer::MatcherString<StringProviderEnd, true>>,
      SequenceRule<PFactory<PascalTokenId::kRealConst>, 
        lexer::MatcherRangeByPredicate<lexer::IsDigit>, 
        lexer::MatcherPredicate<lexer::IsChar<'.'>>, 
        lexer::MatcherRangeByPredicate<lexer::IsDigit>
      >,                 
      SequenceRule<PFactory<PascalTokenId::kIntegerConst>, lexer::MatcherRangeByPredicate<lexer::IsDigit>>, 
      SequenceRule<PFactory<PascalTokenId::kId>, 
        lexer::MatcherPredicate<
          lexer::PredicateOr<
            lexer::IsLetter, 
            lexer::IsChar<'_'>
          >
        >, 
        lexer::MatcherRangeByPredicate<lexer::IsLetterOrDigit>
      >,
      SequenceRule<PFactory<PascalTokenId::kId>, 
        lexer::MatcherPredicate<
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