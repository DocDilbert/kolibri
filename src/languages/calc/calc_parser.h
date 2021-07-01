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

class CalcParserFactory : public parser::IParserFactory<std::shared_ptr<base::Ast<base::MakeShared, CalcToken>>, CalcToken> {
 public:
  using nonterm_type = std::shared_ptr<base::Ast<base::MakeShared, CalcToken>>;
  using term_type = CalcToken;

  CalcParserFactory(base::IAstFactory<nonterm_type, term_type>& ast_factory) : ast_factory_(ast_factory) {}

  nonterm_type CreateNull() override { return ast_factory_.CreateNull(); }
  nonterm_type CreateEmpty(parser::RuleId rule_id) override { return ast_factory_.CreateNop(); }

  nonterm_type CreateTerm(parser::RuleId rule_id, term_type term) override {
    switch (rule_id) {
      case parser::RuleId::kRule2: {
        return ast_factory_.CreateConst(base::ConstType::INTEGER, std::string(term.GetValue()));
      }
      default: {
        return ast_factory_.CreateNull();
      }
        // this should not happen
        return ast_factory_.CreateNull();
    }
  }

  nonterm_type CreateNonTerm(parser::RuleId rule_id, nonterm_type nonterm) override { return ast_factory_.CreateNull(); }

  nonterm_type CreateTermNonTerm(parser::RuleId rule_id, term_type term, nonterm_type nonterm) override {
    switch (rule_id) {
      case parser::RuleId::kRule2: {
        switch (term.GetId()) {
          case CalcTokenId::kPlus: {
            return ast_factory_.CreateUnaryOp(base::UnaryOpType::POSITIVE_OP, nonterm);
          }
          case CalcTokenId::kMinus: {
            return ast_factory_.CreateUnaryOp(base::UnaryOpType::NEGATIVE_OP, nonterm);
          }
          default: {
            return ast_factory_.CreateNull();
          }
        }
      }
      default: {
        return ast_factory_.CreateNull();
      }
    }
    // this should not happen
    return ast_factory_.CreateNull();
  }

  nonterm_type CreateNonTermNonTerm(parser::RuleId rule_id, nonterm_type lhs, nonterm_type rhs) override { return rhs; }

  nonterm_type CreateNonTermTermNonTerm(parser::RuleId rule_id, nonterm_type lhs, term_type term, nonterm_type rhs) override {
    switch (rule_id) {
      case parser::RuleId::kRule0: {
        switch (term.GetId()) {
          case CalcTokenId::kPlus: {
            return ast_factory_.CreateBinaryOp(base::BinaryOpType::ADD, lhs, rhs);
          }
          case CalcTokenId::kMinus: {
            return ast_factory_.CreateBinaryOp(base::BinaryOpType::SUB, lhs, rhs);
          }
          default: {
            return ast_factory_.CreateNull();
          }
        }
      }

      case parser::RuleId::kRule1: {
        switch (term.GetId()) {
          case CalcTokenId::kMultiply: {
            return ast_factory_.CreateBinaryOp(base::BinaryOpType::MUL, lhs, rhs);
          }
          case CalcTokenId::kDivide: {
            return ast_factory_.CreateBinaryOp(base::BinaryOpType::INTEGER_DIV, lhs, rhs);
          }
          default: {
            return ast_factory_.CreateNull();
          }
        }
      }
      default: {
        return ast_factory_.CreateNull();
      }
    }
    // this should not happen
    return ast_factory_.CreateNull();
  }

  nonterm_type CreateNonTermList(parser::RuleId rule_id, std::vector<nonterm_type> statements) override {
    return ast_factory_.CreateCompoundStatement(statements);
  }

  nonterm_type CreateTermNonTermList(parser::RuleId rule_id, std::vector<term_type> terms, std::vector<nonterm_type> nonterms) override {
    return ast_factory_.CreateNull();
  }

 private:
  base::IAstFactory<nonterm_type, term_type>& ast_factory_;
};

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
    parser::OrderedChoiceRules< 
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

using CalcGrammar =
    CalculatorGrammar<std::shared_ptr<base::Ast<base::MakeShared, CalcToken>>, CalcLexer::iterator_type>::type;
using CalcParser = parser::Parser<CalcGrammar>;

}  // namespace calc
}  // namespace languages
#endif