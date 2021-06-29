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
using PascalLexerRules = lexer::LexerRules< 
    PFactory<PascalTokenId::UNKNOWN>,                                                         
    PFactory<PascalTokenId::ENDOFFILE>, 
    lexer::SequenceRule<PFactory<PascalTokenId::LPARENS>, lexer::MatcherPredicate<lexer::IsChar<'('>>>, 
    lexer::SequenceRule<PFactory<PascalTokenId::RPARENS>, lexer::MatcherPredicate<lexer::IsChar<')'>>>,
    lexer::SequenceRule<PFactory<PascalTokenId::PLUS>, lexer::MatcherPredicate<lexer::IsChar<'+'>>>,                     
    lexer::SequenceRule<PFactory<PascalTokenId::MINUS>, lexer::MatcherPredicate<lexer::IsChar<'-'>>>,                    
    lexer::SequenceRule<PFactory<PascalTokenId::MULTIPLY>, lexer::MatcherPredicate<lexer::IsChar<'*'>>>,  
    lexer::SequenceRule<PFactory<PascalTokenId::SEMI>, lexer::MatcherPredicate<lexer::IsChar<';'>>>, 
    lexer::SequenceRule<PFactory<PascalTokenId::DOT>, lexer::MatcherPredicate<lexer::IsChar<'.'>>>,               
    lexer::SequenceRule<PFactory<PascalTokenId::FLOAT_DIV>, lexer::MatcherPredicate<lexer::IsChar<'/'>>>,
    lexer::SequenceRule<PFactory<PascalTokenId::ASSIGN>, lexer::MatcherPredicate<lexer::IsChar<':'>>, lexer::MatcherPredicate<lexer::IsChar<'='>>>,
    lexer::SequenceRule<PFactory<PascalTokenId::COLON>, lexer::MatcherPredicate<lexer::IsChar<':'>>>,
    lexer::SequenceRule<PFactory<PascalTokenId::COMMA>, lexer::MatcherPredicate<lexer::IsChar<','>>>,
    lexer::SequenceRule<PFactory<PascalTokenId::INTEGER_DIV>, lexer::MatcherString<StringProviderDiv, true>>,  
    lexer::SequenceRule<PFactory<PascalTokenId::PROGRAM>, lexer::MatcherString<StringProviderProgram, true>>,
    lexer::SequenceRule<PFactory<PascalTokenId::INTEGER>, lexer::MatcherString<StringProviderInteger, true>>,
    lexer::SequenceRule<PFactory<PascalTokenId::REAL>, lexer::MatcherString<StringProviderReal, true>>,
    lexer::SequenceRule<PFactory<PascalTokenId::VAR>, lexer::MatcherString<StringProviderVar, true>>, 
    lexer::SequenceRule<PFactory<PascalTokenId::BEGIN>, lexer::MatcherString<StringProviderBegin, true>>,    
    lexer::SequenceRule<PFactory<PascalTokenId::END>, lexer::MatcherString<StringProviderEnd, true>>,
    lexer::SequenceRule<PFactory<PascalTokenId::REAL_CONST>, 
      lexer::MatcherRangeByPredicate<lexer::IsDigit>, 
      lexer::MatcherPredicate<lexer::IsChar<'.'>>, 
      lexer::MatcherRangeByPredicate<lexer::IsDigit>
    >,                 
    lexer::SequenceRule<PFactory<PascalTokenId::INTEGER_CONST>, lexer::MatcherRangeByPredicate<lexer::IsDigit>>, 
    lexer::SequenceRule<PFactory<PascalTokenId::ID>, 
      lexer::MatcherPredicate<
        lexer::PredicateOr<
          lexer::IsLetter, 
          lexer::IsChar<'_'>
        >
      >, 
      lexer::MatcherRangeByPredicate<lexer::IsLetterOrDigit>
    >,
    lexer::SequenceRule<PFactory<PascalTokenId::ID>, 
      lexer::MatcherPredicate<
        lexer::PredicateOr<
          lexer::IsLetter, 
          lexer::IsChar<'_'>
        >
      >
    >
>;

using PascalLexer = lexer::Lexer<PascalLexerRules>;

}
}

#endif