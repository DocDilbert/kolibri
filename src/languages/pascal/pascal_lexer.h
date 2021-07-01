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
      PFactory<PascalTokenId::UNKNOWN>,                                                         
      PFactory<PascalTokenId::ENDOFFILE>, 
      SequenceRule<PFactory<PascalTokenId::LPARENS>, lexer::MatcherPredicate<lexer::IsChar<'('>>>, 
      SequenceRule<PFactory<PascalTokenId::RPARENS>, lexer::MatcherPredicate<lexer::IsChar<')'>>>,
      SequenceRule<PFactory<PascalTokenId::PLUS>, lexer::MatcherPredicate<lexer::IsChar<'+'>>>,                     
      SequenceRule<PFactory<PascalTokenId::MINUS>, lexer::MatcherPredicate<lexer::IsChar<'-'>>>,                    
      SequenceRule<PFactory<PascalTokenId::MULTIPLY>, lexer::MatcherPredicate<lexer::IsChar<'*'>>>,  
      SequenceRule<PFactory<PascalTokenId::SEMI>, lexer::MatcherPredicate<lexer::IsChar<';'>>>, 
      SequenceRule<PFactory<PascalTokenId::DOT>, lexer::MatcherPredicate<lexer::IsChar<'.'>>>,               
      SequenceRule<PFactory<PascalTokenId::FLOAT_DIV>, lexer::MatcherPredicate<lexer::IsChar<'/'>>>,
      SequenceRule<PFactory<PascalTokenId::ASSIGN>, lexer::MatcherPredicate<lexer::IsChar<':'>>, lexer::MatcherPredicate<lexer::IsChar<'='>>>,
      SequenceRule<PFactory<PascalTokenId::COLON>, lexer::MatcherPredicate<lexer::IsChar<':'>>>,
      SequenceRule<PFactory<PascalTokenId::COMMA>, lexer::MatcherPredicate<lexer::IsChar<','>>>,
      SequenceRule<PFactory<PascalTokenId::INTEGER_DIV>, lexer::MatcherString<StringProviderDiv, true>>,  
      SequenceRule<PFactory<PascalTokenId::PROGRAM>, lexer::MatcherString<StringProviderProgram, true>>,
      SequenceRule<PFactory<PascalTokenId::INTEGER>, lexer::MatcherString<StringProviderInteger, true>>,
      SequenceRule<PFactory<PascalTokenId::REAL>, lexer::MatcherString<StringProviderReal, true>>,
      SequenceRule<PFactory<PascalTokenId::VAR>, lexer::MatcherString<StringProviderVar, true>>, 
      SequenceRule<PFactory<PascalTokenId::BEGIN>, lexer::MatcherString<StringProviderBegin, true>>,    
      SequenceRule<PFactory<PascalTokenId::END>, lexer::MatcherString<StringProviderEnd, true>>,
      SequenceRule<PFactory<PascalTokenId::REAL_CONST>, 
        lexer::MatcherRangeByPredicate<lexer::IsDigit>, 
        lexer::MatcherPredicate<lexer::IsChar<'.'>>, 
        lexer::MatcherRangeByPredicate<lexer::IsDigit>
      >,                 
      SequenceRule<PFactory<PascalTokenId::INTEGER_CONST>, lexer::MatcherRangeByPredicate<lexer::IsDigit>>, 
      SequenceRule<PFactory<PascalTokenId::ID>, 
        lexer::MatcherPredicate<
          lexer::PredicateOr<
            lexer::IsLetter, 
            lexer::IsChar<'_'>
          >
        >, 
        lexer::MatcherRangeByPredicate<lexer::IsLetterOrDigit>
      >,
      SequenceRule<PFactory<PascalTokenId::ID>, 
        lexer::MatcherPredicate<
          lexer::PredicateOr<
            lexer::IsLetter, 
            lexer::IsChar<'_'>
          >
        >
      >
  >;
};

using PascalLexer = lexer::Lexer<PascalLexerRules::type>;

}
}

#endif