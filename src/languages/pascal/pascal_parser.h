#ifndef INTERTEST_SRC_PASCAL_PARSER_H_
#define INTERTEST_SRC_PASCAL_PARSER_H_

#include "base/ast_factory.h"
#include "base/ast_types.h"
#include "languages/pascal/pascal_lexer.h"
#include "languages/pascal/pascal_token.h"
#include "parser/i_parser_factory.h"
#include "parser/parser.h"
#include "parser/parser_productions.h"
#include "parser/parser_rules.h"

// clang-format on

namespace languages {
namespace pascal {

template <languages::pascal::PascalTokenId Id>
class PascalTokenPredicate {
 public:
  bool operator()(languages::pascal::PascalToken token) { return token.GetId() == Id; };
};

class PascalParserFactory
    : public parser::IParserFactory<std::shared_ptr<base::Ast<base::MakeShared, languages::pascal::PascalToken>>, languages::pascal::PascalToken> {
 public:
  using nonterm_type = std::shared_ptr<base::Ast<base::MakeShared, languages::pascal::PascalToken>>;
  using term_type = languages::pascal::PascalToken;

  PascalParserFactory(base::IAstFactory<nonterm_type, term_type>& ast_factory) : ast_factory_(ast_factory) {}

  nonterm_type CreateNull() override { return ast_factory_.CreateNull(); }

  nonterm_type CreateEmpty(parser::RuleId rule_id) override { return ast_factory_.CreateNop(); }

  nonterm_type CreateTerm(parser::RuleId rule_id, term_type term) override {
    switch (rule_id) {
      case parser::RuleId::kRule4: {  // type_spec
        return ast_factory_.CreateRaw(term);
      }
      case parser::RuleId::kRule12: {  // factor
        switch (term.GetId()) {
          case languages::pascal::PascalTokenId::kIntegerConst: {
            return ast_factory_.CreateConst(base::ConstType::INTEGER, std::string(term.GetValue()));
          }
          case languages::pascal::PascalTokenId::kRealConst: {
            return ast_factory_.CreateConst(base::ConstType::REAL, std::string(term.GetValue()));
          }
          default: {
            return ast_factory_.CreateNull();
          }
        }
      }
      case parser::RuleId::kRule13: {  // variable
        auto var_name = std::string(term.GetValue());
        std::for_each(var_name.begin(), var_name.end(), [](char& c) { c = ::tolower(c); });
        return ast_factory_.CreateId(var_name);
      }
      default: {
        return ast_factory_.CreateNull();
      }
    }

    // this should not happen
    return ast_factory_.CreateNull();
  }
  nonterm_type CreateNonTerm(parser::RuleId rule_id, nonterm_type nonterm) override {
    switch (rule_id) {
      case parser::RuleId::kRule5: {  // compound_statement
        assert(nonterm->GetTypeId() == base::AstTypeId::kAstRawListType);
        auto& raw_list = dynamic_cast<base::AstRawList<base::MakeShared, term_type>&>(*nonterm);
        return ast_factory_.CreateCompoundStatement(raw_list.Get());
      }

      default: {
        return ast_factory_.CreateNull();
      }
    }
    return ast_factory_.CreateNull();
  }

  nonterm_type CreateTermNonTerm(parser::RuleId rule_id, term_type term, nonterm_type nonterm) override {
    switch (rule_id) {
      case parser::RuleId::kRule12: {  // factor
        switch (term.GetId()) {
          case languages::pascal::PascalTokenId::kPlus: {
            return ast_factory_.CreateUnaryOp(base::UnaryOpType::POSITIVE_OP, nonterm);
          }
          case languages::pascal::PascalTokenId::kMinus: {
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
  nonterm_type CreateNonTermNonTerm(parser::RuleId rule_id, nonterm_type lhs, nonterm_type rhs) override {
    switch (rule_id) {
      case parser::RuleId::kRule0: {  // program
        return ast_factory_.CreateProgram(lhs, rhs);
      }
      case parser::RuleId::kRule1: {  // block
        assert(lhs->GetTypeId() == base::AstTypeId::kAstRawListType);
        auto& var_decl_list = dynamic_cast<base::AstRawList<base::MakeShared, term_type>&>(*lhs);

        return ast_factory_.CreateBlock(var_decl_list.Get(), rhs);
      }
      default: {
        return ast_factory_.CreateNull();
      }
    }
    return ast_factory_.CreateNull();
  }

  nonterm_type CreateNonTermTermNonTerm(parser::RuleId rule_id, nonterm_type lhs, term_type term, nonterm_type rhs) override {
    switch (rule_id) {
      case parser::RuleId::kRule8: {  // assignment_statement
        switch (term.GetId()) {
          case languages::pascal::PascalTokenId::kAssign: {
            return ast_factory_.CreateBinaryOp(base::BinaryOpType::ASSSIGN, lhs, rhs);
          }
          default: {
            return ast_factory_.CreateNull();
          }
        }
      }

      case parser::RuleId::kRule10: {  // expr
        switch (term.GetId()) {
          case languages::pascal::PascalTokenId::kPlus: {
            return ast_factory_.CreateBinaryOp(base::BinaryOpType::ADD, lhs, rhs);
          }
          case languages::pascal::PascalTokenId::kMinus: {
            return ast_factory_.CreateBinaryOp(base::BinaryOpType::SUB, lhs, rhs);
          }
          default: {
            return ast_factory_.CreateNull();
          }
        }
      }

      case parser::RuleId::kRule11: {  // term
        switch (term.GetId()) {
          case languages::pascal::PascalTokenId::kMultiply: {
            return ast_factory_.CreateBinaryOp(base::BinaryOpType::MUL, lhs, rhs);
          }
          case languages::pascal::PascalTokenId::kIntegerDiv: {
            return ast_factory_.CreateBinaryOp(base::BinaryOpType::INTEGER_DIV, lhs, rhs);
          }
          case languages::pascal::PascalTokenId::kFloatDiv: {
            return ast_factory_.CreateBinaryOp(base::BinaryOpType::FLOAT_DIV, lhs, rhs);
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
  }

  nonterm_type CreateNonTermList(parser::RuleId rule_id, std::vector<nonterm_type> nonterms) override {
    switch (rule_id) {
      case parser::RuleId::kRule2: {  // declarations
        std::vector<nonterm_type> var_decls;

        for (int i = 0; i < nonterms.size(); ++i) {
          auto nonterm = nonterms[i];
          if (nonterm->GetTypeId() == base::AstTypeId::kAstRawListType) {
            auto& var_decl_list = dynamic_cast<base::AstRawList<base::MakeShared, term_type>&>(*nonterm);
            auto var_decl_vector = var_decl_list.Get();
            var_decls.insert(var_decls.end(), var_decl_vector.begin(), var_decl_vector.end());
          }
        }
        return ast_factory_.CreateRawList(var_decls);
      }
      case parser::RuleId::kRule6: {
        return ast_factory_.CreateRawList(nonterms);
      }

      default: {
        return ast_factory_.CreateNull();
      }
        return ast_factory_.CreateNull();
    }
  }

  nonterm_type CreateTermNonTermList(parser::RuleId rule_id, std::vector<term_type> terms, std::vector<nonterm_type> nonterms) override {
    std::vector<term_type> id_terms;

    for (int i = 0; i < terms.size(); ++i) {
      auto term = terms[i];
      if (term.GetId() == languages::pascal::PascalTokenId::kId) {
        id_terms.push_back(term);
      }
    }

    assert(nonterms.size() == 1);
    assert(nonterms[0]->GetTypeId() == base::AstTypeId::kAstRawType);

    // extract from Raw node
    auto type_term = dynamic_cast<base::AstRaw<base::MakeShared, term_type>&>(*nonterms[0]).GetTerm();

    std::vector<nonterm_type> var_decls;
    for (int i = 0; i < id_terms.size(); ++i) {
      auto id = id_terms[i];
      var_decls.push_back(ast_factory_.CreateVariableDeclaration(id, type_term));
    }
    return ast_factory_.CreateRawList(var_decls);
  }

 private:
  base::IAstFactory<nonterm_type, term_type>& ast_factory_;
};

// clang-format off
  
template <typename TNonTerm, typename Iterator>
using PascalGrammar = parser::ParserGrammar<
    TNonTerm, Iterator,
 
    // Rule #0 - program
    // program : PROGRAM variable SEMI block DOT
    parser::Rule<parser::NonTermNonTermProduction, 
      parser::SequenceExpr<
        parser::TermExpr<PascalTokenPredicate<languages::pascal::PascalTokenId::kProgram>>,
        parser::NonTermExpr<parser::RuleId::kRule13>, 
        parser::TermExpr<PascalTokenPredicate<languages::pascal::PascalTokenId::kSemi>>,
        parser::NonTermExpr<parser::RuleId::kRule1>, 
        parser::TermExpr<PascalTokenPredicate<languages::pascal::PascalTokenId::kDot>>
      >
    >,

    // Rule #1 - block
    // block : declarations compound_statement
    parser::Rule<parser::NonTermNonTermProduction, 
      parser::SequenceExpr<
        parser::NonTermExpr<parser::RuleId::kRule2>, 
        parser::NonTermExpr<parser::RuleId::kRule5>
      >
    >,

    // Rule #2 - declarations
    // declarations : VAR (variable_declaration SEMI)+
    //              | empty
    parser::Rule<parser::NonTermListProduction, 
      parser::OrderedChoiceExpr<
        parser::SequenceExpr<
          parser::TermExpr<PascalTokenPredicate<languages::pascal::PascalTokenId::kVar>>,
          parser::NMatchesOrMoreExpr<1,
              parser::SequenceExpr<
                parser::NonTermExpr<parser::RuleId::kRule3>,
                parser::TermExpr<PascalTokenPredicate<languages::pascal::PascalTokenId::kSemi>>
              >
          >
        >,
        parser::NonTermExpr<parser::RuleId::kRule9>
      >
    >,

    // Rule #3
    // variable_declaration : ID (COMMA ID)* COLON type_spec
    parser::Rule<parser::TermNonTermListProduction, 
      parser::SequenceExpr<
        parser::TermExpr<PascalTokenPredicate<languages::pascal::PascalTokenId::kId>>,
        parser::NMatchesOrMoreExpr<0,
          parser::SequenceExpr<
            parser::TermExpr<PascalTokenPredicate<languages::pascal::PascalTokenId::kComma>>,
            parser::TermExpr<PascalTokenPredicate<languages::pascal::PascalTokenId::kId>>
          >
        >,
        parser::TermExpr<PascalTokenPredicate<languages::pascal::PascalTokenId::kColon>>,
        parser::NonTermExpr<parser::RuleId::kRule4>
      >
    >,
 
    // Rule #4 - type_spec
    // type_spec : INTEGER | REAL
    parser::Rule<parser::TermProduction, 
      parser::OrderedChoiceExpr<
        parser::TermExpr<PascalTokenPredicate<languages::pascal::PascalTokenId::kInteger>>,
        parser::TermExpr<PascalTokenPredicate<languages::pascal::PascalTokenId::kReal>>
      >
    >,

    // Rule #5 - compound_statement
    // compound_statement : BEGIN statement_list END
    parser::Rule<parser::NonTermProduction, 
      parser::SequenceExpr<parser::TermExpr<PascalTokenPredicate<languages::pascal::PascalTokenId::kBegin>>, parser::NonTermExpr<parser::RuleId::kRule6>, parser::TermExpr<PascalTokenPredicate<languages::pascal::PascalTokenId::kEnd>>>
    >,

    // Rule #6 - statement_list
    // statement_list : statement
    //                | statement SEMI statement_list
    // Alternativ:
    //
    // statement_list : statement (SEMI statement)*
    parser::Rule<parser::NonTermListProduction, 
      parser::SequenceExpr<
        parser::NonTermExpr<parser::RuleId::kRule7>,
        parser::NMatchesOrMoreExpr<0,
          parser::SequenceExpr<
            parser::TermExpr<PascalTokenPredicate<languages::pascal::PascalTokenId::kSemi>>, 
            parser::NonTermExpr<parser::RuleId::kRule7>
          >
        >
      >
    >,

    // Rule #7 - statement
    // statement : compound_statement
    //          | assignment_statement
    //          | empty
    parser::OrderedChoiceRules< 
      parser::Rule<parser::BypassLastTermProduction, parser::NonTermExpr<parser::RuleId::kRule5>>,
      parser::Rule<parser::BypassLastTermProduction, parser::NonTermExpr<parser::RuleId::kRule8>>,
      parser::Rule<parser::BypassLastTermProduction, parser::NonTermExpr<parser::RuleId::kRule9>>
    >,

    // Rule #8 - assignment_statement
    // assignment_statement : variable ASSIGN expr
    parser::Rule<parser::NonTermTermNonTermProduction,
      parser::SequenceExpr< parser::NonTermExpr<parser::RuleId::kRule13>, parser::TermExpr<PascalTokenPredicate<languages::pascal::PascalTokenId::kAssign>>, parser::NonTermExpr<parser::RuleId::kRule10>>
    >,

    // Rule #9 - empty
    // empty :
    parser::Rule<parser::EmptyProduction, parser::EmptyExpr>,

    // Rule #10 - expr
    // expr : term ((kPlus | kMinus) term)*
    parser::Rule<parser::NonTermTermNonTermSequenceProduction,
      parser::SequenceExpr<
        parser::NonTermExpr<parser::RuleId::kRule11>,
        parser::NMatchesOrMoreExpr<0,
          parser::SequenceExpr<
            parser::OrderedChoiceExpr<
              parser::TermExpr<PascalTokenPredicate<languages::pascal::PascalTokenId::kPlus>>, 
              parser::TermExpr<PascalTokenPredicate<languages::pascal::PascalTokenId::kMinus>>
            >, 
            parser::NonTermExpr<parser::RuleId::kRule11>
          >
        >
      >
    >,

    // Rule #11 - term
    // term : factor ((MUL | INTEGER_DIV | FLOAT_DIV) factor)*
    parser::Rule<parser::NonTermTermNonTermSequenceProduction,
      parser::SequenceExpr<
        parser::NonTermExpr<parser::RuleId::kRule12>,
        parser::NMatchesOrMoreExpr<0,
          parser::SequenceExpr<
            parser::OrderedChoiceExpr<
              parser::TermExpr<PascalTokenPredicate<languages::pascal::PascalTokenId::kMultiply>>, 
              parser::TermExpr<PascalTokenPredicate<languages::pascal::PascalTokenId::kIntegerDiv>>,
              parser::TermExpr<PascalTokenPredicate<languages::pascal::PascalTokenId::kFloatDiv>>
            >, 
            parser::NonTermExpr<parser::RuleId::kRule12>
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
    parser::OrderedChoiceRules< 
        parser::Rule<parser::TermNonTermProduction, 
          parser::SequenceExpr<parser::TermExpr<PascalTokenPredicate<languages::pascal::PascalTokenId::kPlus>>, parser::NonTermExpr<parser::RuleId::kRule12>>
        >,
        parser::Rule<parser::TermNonTermProduction, 
          parser::SequenceExpr<parser::TermExpr<PascalTokenPredicate<languages::pascal::PascalTokenId::kMinus>>, parser::NonTermExpr<parser::RuleId::kRule12>>
        >,
        parser::Rule<parser::TermProduction, 
          parser::TermExpr<PascalTokenPredicate<languages::pascal::PascalTokenId::kIntegerConst>>
        >,
        parser::Rule<parser::TermProduction, 
          parser::TermExpr<PascalTokenPredicate<languages::pascal::PascalTokenId::kRealConst>>
        >,
        parser::Rule<parser::BypassLastTermProduction,
          parser::SequenceExpr<parser::TermExpr<PascalTokenPredicate<languages::pascal::PascalTokenId::kLParens>>, parser::NonTermExpr<parser::RuleId::kRule10>, parser::TermExpr<PascalTokenPredicate<languages::pascal::PascalTokenId::kRParens>>>
        >,
        parser::Rule<parser::BypassLastTermProduction, 
          parser::NonTermExpr<parser::RuleId::kRule13>
        >
    >,

    // Rule #13 - variable
    // variable: ID
    parser::Rule<parser::TermProduction, parser::TermExpr<PascalTokenPredicate<languages::pascal::PascalTokenId::kId>>> 
  >;

// clang-format on

using PGrammar = PascalGrammar<std::shared_ptr<base::Ast<base::MakeShared, PascalToken>>, PascalLexer::iterator_type>;
using PascalParser = parser::Parser<PGrammar>;

}  // namespace pascal
}  // namespace languages
#endif