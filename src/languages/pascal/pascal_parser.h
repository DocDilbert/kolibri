#ifndef KOLIBRI_SRC_PASCAL_PARSER_H_
#define KOLIBRI_SRC_PASCAL_PARSER_H_

#include "base/ast_factory.h"
#include "base/ast_types.h"
#include "languages/pascal/pascal_lexer.h"
#include "languages/pascal/pascal_token.h"
#include "parser/i_parser_factory.h"
#include "parser/parser.h"
#include "parser/parser_productions.h"
#include "parser/parser_rules.h"

namespace languages {
namespace pascal {

template <PascalTokenId Id>
class PascalTokenPredicate {
 public:
  bool operator()(PascalToken token) { return token.GetId() == Id; };
};

template <typename TNonTerm, typename Iterator>
struct PascalGrammar : public parser::GrammarBase {
  // clang-format off
  using type = parser::ParserGrammar<
    TNonTerm, Iterator,
 
    // Rule #0 - program
    // program : PROGRAM variable SEMI block DOT
    Rule<parser::NonTermNonTermProduction, 
      SequenceExpr<
        TermExpr<PascalTokenPredicate<PascalTokenId::kProgram>>,
        NonTermExpr<parser::RuleId::kRule13>, 
        TermExpr<PascalTokenPredicate<PascalTokenId::kSemi>>,
        NonTermExpr<parser::RuleId::kRule1>, 
        TermExpr<PascalTokenPredicate<PascalTokenId::kDot>>
      >
    >,

    // Rule #1 - block
    // block : declarations compound_statement
    Rule<parser::NonTermNonTermProduction, 
      SequenceExpr<
        NonTermExpr<parser::RuleId::kRule2>, 
        NonTermExpr<parser::RuleId::kRule5>
      >
    >,

    // Rule #2 - declarations
    // declarations : VAR (variable_declaration SEMI)+
    //              | empty
    Rule<parser::NonTermListProduction, 
      OrderedChoiceExpr<
        SequenceExpr<
          TermExpr<PascalTokenPredicate<PascalTokenId::kVar>>,
          NMatchesOrMoreExpr<1,
              SequenceExpr<
                NonTermExpr<parser::RuleId::kRule3>,
                TermExpr<PascalTokenPredicate<PascalTokenId::kSemi>>
              >
          >
        >,
        NonTermExpr<parser::RuleId::kRule9>
      >
    >,

    // Rule #3
    // variable_declaration : ID (COMMA ID)* COLON type_spec
    Rule<parser::TermNonTermListProduction, 
      SequenceExpr<
        TermExpr<PascalTokenPredicate<PascalTokenId::kId>>,
        NMatchesOrMoreExpr<0,
          SequenceExpr<
            TermExpr<PascalTokenPredicate<PascalTokenId::kComma>>,
            TermExpr<PascalTokenPredicate<PascalTokenId::kId>>
          >
        >,
        TermExpr<PascalTokenPredicate<PascalTokenId::kColon>>,
        NonTermExpr<parser::RuleId::kRule4>
      >
    >,
 
    // Rule #4 - type_spec
    // type_spec : INTEGER | REAL
    Rule<parser::TermProduction, 
      OrderedChoiceExpr<
        TermExpr<PascalTokenPredicate<PascalTokenId::kInteger>>,
        TermExpr<PascalTokenPredicate<PascalTokenId::kReal>>
      >
    >,

    // Rule #5 - compound_statement
    // compound_statement : BEGIN statement_list END
    Rule<parser::NonTermProduction, 
      SequenceExpr<
        TermExpr<PascalTokenPredicate<PascalTokenId::kBegin>>, 
        NonTermExpr<parser::RuleId::kRule6>, 
        TermExpr<PascalTokenPredicate<PascalTokenId::kEnd>>
      >
    >,

    // Rule #6 - statement_list
    // statement_list : statement
    //                | statement SEMI statement_list
    // Alternativ:
    //
    // statement_list : statement (SEMI statement)*
    Rule<parser::NonTermListProduction, 
      SequenceExpr<
        NonTermExpr<parser::RuleId::kRule7>,
        NMatchesOrMoreExpr<0,
          SequenceExpr<
            TermExpr<PascalTokenPredicate<PascalTokenId::kSemi>>, 
            NonTermExpr<parser::RuleId::kRule7>
          >
        >
      >
    >,

    // Rule #7 - statement
    // statement : compound_statement
    //          | assignment_statement
    //          | empty
    OrderedChoiceRules< 
      Rule<parser::BypassLastTermProduction, NonTermExpr<parser::RuleId::kRule5>>,
      Rule<parser::BypassLastTermProduction, NonTermExpr<parser::RuleId::kRule8>>,
      Rule<parser::BypassLastTermProduction, NonTermExpr<parser::RuleId::kRule9>>
    >,

    // Rule #8 - assignment_statement
    // assignment_statement : variable ASSIGN expr
    Rule<parser::NonTermTermNonTermProduction,
      SequenceExpr< NonTermExpr<parser::RuleId::kRule13>, TermExpr<PascalTokenPredicate<PascalTokenId::kAssign>>, NonTermExpr<parser::RuleId::kRule10>>
    >,

    // Rule #9 - empty
    // empty :
    Rule<parser::EmptyProduction, EmptyExpr>,

    // Rule #10 - expr
    // expr : term ((kPlus | kMinus) term)*
    Rule<parser::NonTermTermNonTermSequenceProduction,
      SequenceExpr<
        NonTermExpr<parser::RuleId::kRule11>,
        NMatchesOrMoreExpr<0,
          SequenceExpr<
            OrderedChoiceExpr<
              TermExpr<PascalTokenPredicate<PascalTokenId::kPlus>>, 
              TermExpr<PascalTokenPredicate<PascalTokenId::kMinus>>
            >, 
            NonTermExpr<parser::RuleId::kRule11>
          >
        >
      >
    >,

    // Rule #11 - term
    // term : factor ((MUL | INTEGER_DIV | FLOAT_DIV) factor)*
    Rule<parser::NonTermTermNonTermSequenceProduction,
      SequenceExpr<
        NonTermExpr<parser::RuleId::kRule12>,
        NMatchesOrMoreExpr<0,
          SequenceExpr<
            OrderedChoiceExpr<
              TermExpr<PascalTokenPredicate<PascalTokenId::kMultiply>>, 
              TermExpr<PascalTokenPredicate<PascalTokenId::kIntegerDiv>>,
              TermExpr<PascalTokenPredicate<PascalTokenId::kFloatDiv>>
            >, 
            NonTermExpr<parser::RuleId::kRule12>
          >
        >
      >
    >,

    // Rule #12 - factor
    // factor : kPlus factor
    //        | kMinus factor
    //        | INTEGER_CONST
    //        | REAL_CONST
    //        | LPAREN expr RPAREN
    //        | variable
    OrderedChoiceRules< 
        Rule<parser::TermNonTermProduction, 
          SequenceExpr<TermExpr<PascalTokenPredicate<PascalTokenId::kPlus>>, NonTermExpr<parser::RuleId::kRule12>>
        >,
        Rule<parser::TermNonTermProduction, 
          SequenceExpr<TermExpr<PascalTokenPredicate<PascalTokenId::kMinus>>, NonTermExpr<parser::RuleId::kRule12>>
        >,
        Rule<parser::TermProduction, 
          TermExpr<PascalTokenPredicate<PascalTokenId::kIntegerConst>>
        >,
        Rule<parser::TermProduction, 
          TermExpr<PascalTokenPredicate<PascalTokenId::kRealConst>>
        >,
        Rule<parser::BypassLastTermProduction,
          SequenceExpr<TermExpr<PascalTokenPredicate<PascalTokenId::kLParens>>, NonTermExpr<parser::RuleId::kRule10>, TermExpr<PascalTokenPredicate<PascalTokenId::kRParens>>>
        >,
        Rule<parser::BypassLastTermProduction, 
          NonTermExpr<parser::RuleId::kRule13>
        >
    >,

    // Rule #13 - variable
    // variable: ID
    Rule<parser::TermProduction, TermExpr<PascalTokenPredicate<PascalTokenId::kId>>> 
  >;

  // clang-format on
};
using PascGrammar = PascalGrammar<std::shared_ptr<base::Ast<base::MakeShared, PascalToken>>, PascalLexer::iterator_type>::type;
using PascalParser = parser::Parser<PascGrammar>;

}  // namespace pascal
}  // namespace languages
#endif