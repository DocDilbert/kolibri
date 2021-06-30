#ifndef INTERTEST_SRC_PASCAL_PARSER_H_
#define INTERTEST_SRC_PASCAL_PARSER_H_

#include "base/ast_factory.h"
#include "base/ast_types.h"
#include "languages/pascal/pascal_token.h"
#include "languages/pascal/pascal_lexer.h"
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
          case languages::pascal::PascalTokenId::INTEGER_CONST: {
            return ast_factory_.CreateConst(base::ConstType::INTEGER, std::string(term.GetValue()));
          }
          case languages::pascal::PascalTokenId::REAL_CONST: {
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
          case languages::pascal::PascalTokenId::PLUS: {
            return ast_factory_.CreateUnaryOp(base::UnaryOpType::POSITIVE_OP, nonterm);
          }
          case languages::pascal::PascalTokenId::MINUS: {
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
          case languages::pascal::PascalTokenId::ASSIGN: {
            return ast_factory_.CreateBinaryOp(base::BinaryOpType::ASSSIGN, lhs, rhs);
          }
          default: {
            return ast_factory_.CreateNull();
          }
        }
      }

      case parser::RuleId::kRule10: {  // expr
        switch (term.GetId()) {
          case languages::pascal::PascalTokenId::PLUS: {
            return ast_factory_.CreateBinaryOp(base::BinaryOpType::ADD, lhs, rhs);
          }
          case languages::pascal::PascalTokenId::MINUS: {
            return ast_factory_.CreateBinaryOp(base::BinaryOpType::SUB, lhs, rhs);
          }
          default: {
            return ast_factory_.CreateNull();
          }
        }
      }

      case parser::RuleId::kRule11: {  // term
        switch (term.GetId()) {
          case languages::pascal::PascalTokenId::MULTIPLY: {
            return ast_factory_.CreateBinaryOp(base::BinaryOpType::MUL, lhs, rhs);
          }
          case languages::pascal::PascalTokenId::INTEGER_DIV: {
            return ast_factory_.CreateBinaryOp(base::BinaryOpType::INTEGER_DIV, lhs, rhs);
          }
          case languages::pascal::PascalTokenId::FLOAT_DIV: {
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
      if (term.GetId() == languages::pascal::PascalTokenId::ID) {
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
        parser::TermExpr<PascalTokenPredicate<languages::pascal::PascalTokenId::PROGRAM>>,
        parser::NonTermExpr<parser::RuleId::kRule13>, 
        parser::TermExpr<PascalTokenPredicate<languages::pascal::PascalTokenId::SEMI>>,
        parser::NonTermExpr<parser::RuleId::kRule1>, 
        parser::TermExpr<PascalTokenPredicate<languages::pascal::PascalTokenId::DOT>>
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
          parser::TermExpr<PascalTokenPredicate<languages::pascal::PascalTokenId::VAR>>,
          parser::NMatchesOrMoreExpr<1,
              parser::SequenceExpr<
                parser::NonTermExpr<parser::RuleId::kRule3>,
                parser::TermExpr<PascalTokenPredicate<languages::pascal::PascalTokenId::SEMI>>
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
        parser::TermExpr<PascalTokenPredicate<languages::pascal::PascalTokenId::ID>>,
        parser::NMatchesOrMoreExpr<0,
          parser::SequenceExpr<
            parser::TermExpr<PascalTokenPredicate<languages::pascal::PascalTokenId::COMMA>>,
            parser::TermExpr<PascalTokenPredicate<languages::pascal::PascalTokenId::ID>>
          >
        >,
        parser::TermExpr<PascalTokenPredicate<languages::pascal::PascalTokenId::COLON>>,
        parser::NonTermExpr<parser::RuleId::kRule4>
      >
    >,
 
    // Rule #4 - type_spec
    // type_spec : INTEGER | REAL
    parser::Rule<parser::TermProduction, 
      parser::OrderedChoiceExpr<
        parser::TermExpr<PascalTokenPredicate<languages::pascal::PascalTokenId::INTEGER>>,
        parser::TermExpr<PascalTokenPredicate<languages::pascal::PascalTokenId::REAL>>
      >
    >,

    // Rule #5 - compound_statement
    // compound_statement : BEGIN statement_list END
    parser::Rule<parser::NonTermProduction, 
      parser::SequenceExpr<parser::TermExpr<PascalTokenPredicate<languages::pascal::PascalTokenId::BEGIN>>, parser::NonTermExpr<parser::RuleId::kRule6>, parser::TermExpr<PascalTokenPredicate<languages::pascal::PascalTokenId::END>>>
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
            parser::TermExpr<PascalTokenPredicate<languages::pascal::PascalTokenId::SEMI>>, 
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
      parser::SequenceExpr< parser::NonTermExpr<parser::RuleId::kRule13>, parser::TermExpr<PascalTokenPredicate<languages::pascal::PascalTokenId::ASSIGN>>, parser::NonTermExpr<parser::RuleId::kRule10>>
    >,

    // Rule #9 - empty
    // empty :
    parser::Rule<parser::EmptyProduction, parser::EmptyExpr>,

    // Rule #10 - expr
    // expr : term ((PLUS | MINUS) term)*
    parser::Rule<parser::NonTermTermNonTermSequenceProduction,
      parser::SequenceExpr<
        parser::NonTermExpr<parser::RuleId::kRule11>,
        parser::NMatchesOrMoreExpr<0,
          parser::SequenceExpr<
            parser::OrderedChoiceExpr<
              parser::TermExpr<PascalTokenPredicate<languages::pascal::PascalTokenId::PLUS>>, 
              parser::TermExpr<PascalTokenPredicate<languages::pascal::PascalTokenId::MINUS>>
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
              parser::TermExpr<PascalTokenPredicate<languages::pascal::PascalTokenId::MULTIPLY>>, 
              parser::TermExpr<PascalTokenPredicate<languages::pascal::PascalTokenId::INTEGER_DIV>>,
              parser::TermExpr<PascalTokenPredicate<languages::pascal::PascalTokenId::FLOAT_DIV>>
            >, 
            parser::NonTermExpr<parser::RuleId::kRule12>
          >
        >
      >
    >,

    // Rule #12 - factor
    // factor : PLUS factor
    //        | MINUS factor
    //        | INTEGER_CONST
    //        | REAL_CONST
    //        | LPAREN expr RPAREN
    //        | variable
    parser::OrderedChoiceRules< 
        parser::Rule<parser::TermNonTermProduction, 
          parser::SequenceExpr<parser::TermExpr<PascalTokenPredicate<languages::pascal::PascalTokenId::PLUS>>, parser::NonTermExpr<parser::RuleId::kRule12>>
        >,
        parser::Rule<parser::TermNonTermProduction, 
          parser::SequenceExpr<parser::TermExpr<PascalTokenPredicate<languages::pascal::PascalTokenId::MINUS>>, parser::NonTermExpr<parser::RuleId::kRule12>>
        >,
        parser::Rule<parser::TermProduction, 
          parser::TermExpr<PascalTokenPredicate<languages::pascal::PascalTokenId::INTEGER_CONST>>
        >,
        parser::Rule<parser::TermProduction, 
          parser::TermExpr<PascalTokenPredicate<languages::pascal::PascalTokenId::REAL_CONST>>
        >,
        parser::Rule<parser::BypassLastTermProduction,
          parser::SequenceExpr<parser::TermExpr<PascalTokenPredicate<languages::pascal::PascalTokenId::LPARENS>>, parser::NonTermExpr<parser::RuleId::kRule10>, parser::TermExpr<PascalTokenPredicate<languages::pascal::PascalTokenId::RPARENS>>>
        >,
        parser::Rule<parser::BypassLastTermProduction, 
          parser::NonTermExpr<parser::RuleId::kRule13>
        >
    >,

    // Rule #13 - variable
    // variable: ID
    parser::Rule<parser::TermProduction, parser::TermExpr<PascalTokenPredicate<languages::pascal::PascalTokenId::ID>>> 
  >;

// clang-format on

using PGrammar = PascalGrammar<std::shared_ptr<base::Ast<base::MakeShared, PascalToken>>, PascalLexer::iterator_type>;
using PascalParser = parser::Parser<PGrammar>;

}
}
#endif