#ifndef INTERTEST_SRC_PASCAL_PARSER_H_
#define INTERTEST_SRC_PASCAL_PARSER_H_

#include "base/ast_factory.h"
#include "base/ast_types.h"
#include "languages/pascal/pascal_token.h"
#include "parser/i_parser_factory.h"
#include "parser/parser.h"
#include "parser/parser_productions.h"
#include "parser/parser_rules.h"

// clang-format on
// TODO: Eigene Datei für Parser Factory
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
namespace parser {
  
template <typename TNonTerm, typename Iterator>
using PascalGrammar = ParserGrammar<
    TNonTerm, Iterator,
 
    // Rule #0 - program
    // program : PROGRAM variable SEMI block DOT
    Rule<NonTermNonTermProduction, 
      SequenceExpr<
        TermExpr<PascalTokenPredicate<languages::pascal::PascalTokenId::PROGRAM>>,
        NonTermExpr<RuleId::kRule13>, 
        TermExpr<PascalTokenPredicate<languages::pascal::PascalTokenId::SEMI>>,
        NonTermExpr<RuleId::kRule1>, 
        TermExpr<PascalTokenPredicate<languages::pascal::PascalTokenId::DOT>>
      >
    >,

    // Rule #1 - block
    // block : declarations compound_statement
    Rule<NonTermNonTermProduction, 
      SequenceExpr<
        NonTermExpr<RuleId::kRule2>, 
        NonTermExpr<RuleId::kRule5>
      >
    >,

    // Rule #2 - declarations
    // declarations : VAR (variable_declaration SEMI)+
    //              | empty
    Rule<NonTermListProduction, 
      OrderedChoiceExpr<
        SequenceExpr<
          TermExpr<PascalTokenPredicate<languages::pascal::PascalTokenId::VAR>>,
          NMatchesOrMoreExpr<1,
              SequenceExpr<
                NonTermExpr<RuleId::kRule3>,
                TermExpr<PascalTokenPredicate<languages::pascal::PascalTokenId::SEMI>>
              >
          >
        >,
        NonTermExpr<RuleId::kRule9>
      >
    >,

    // Rule #3
    // variable_declaration : ID (COMMA ID)* COLON type_spec
    Rule<TermNonTermListProduction, 
      SequenceExpr<
        TermExpr<PascalTokenPredicate<languages::pascal::PascalTokenId::ID>>,
        NMatchesOrMoreExpr<0,
          SequenceExpr<
            TermExpr<PascalTokenPredicate<languages::pascal::PascalTokenId::COMMA>>,
            TermExpr<PascalTokenPredicate<languages::pascal::PascalTokenId::ID>>
          >
        >,
        TermExpr<PascalTokenPredicate<languages::pascal::PascalTokenId::COLON>>,
        NonTermExpr<RuleId::kRule4>
      >
    >,
 
    // Rule #4 - type_spec
    // type_spec : INTEGER | REAL
    Rule<TermProduction, 
      OrderedChoiceExpr<
        TermExpr<PascalTokenPredicate<languages::pascal::PascalTokenId::INTEGER>>,
        TermExpr<PascalTokenPredicate<languages::pascal::PascalTokenId::REAL>>
      >
    >,

    // Rule #5 - compound_statement
    // compound_statement : BEGIN statement_list END
    Rule<NonTermProduction, 
      SequenceExpr<TermExpr<PascalTokenPredicate<languages::pascal::PascalTokenId::BEGIN>>, NonTermExpr<RuleId::kRule6>, TermExpr<PascalTokenPredicate<languages::pascal::PascalTokenId::END>>>
    >,

    // Rule #6 - statement_list
    // statement_list : statement
    //                | statement SEMI statement_list
    // Alternativ:
    //
    // statement_list : statement (SEMI statement)*
    Rule<NonTermListProduction, 
      SequenceExpr<
        NonTermExpr<RuleId::kRule7>,
        NMatchesOrMoreExpr<0,
          SequenceExpr<
            TermExpr<PascalTokenPredicate<languages::pascal::PascalTokenId::SEMI>>, 
            NonTermExpr<RuleId::kRule7>
          >
        >
      >
    >,

    // Rule #7 - statement
    // statement : compound_statement
    //          | assignment_statement
    //          | empty
    OrderedChoiceRules< 
      Rule<BypassLastTermProduction, NonTermExpr<RuleId::kRule5>>,
      Rule<BypassLastTermProduction, NonTermExpr<RuleId::kRule8>>,
      Rule<BypassLastTermProduction, NonTermExpr<RuleId::kRule9>>
    >,

    // Rule #8 - assignment_statement
    // assignment_statement : variable ASSIGN expr
    Rule<NonTermTermNonTermProduction,
      SequenceExpr< NonTermExpr<RuleId::kRule13>, TermExpr<PascalTokenPredicate<languages::pascal::PascalTokenId::ASSIGN>>, NonTermExpr<RuleId::kRule10>>
    >,

    // Rule #9 - empty
    // empty :
    Rule<EmptyProduction, EmptyExpr>,

    // Rule #10 - expr
    // expr : term ((PLUS | MINUS) term)*
    Rule<NonTermTermNonTermSequenceProduction,
      SequenceExpr<
        NonTermExpr<RuleId::kRule11>,
        NMatchesOrMoreExpr<0,
          SequenceExpr<
            OrderedChoiceExpr<
              TermExpr<PascalTokenPredicate<languages::pascal::PascalTokenId::PLUS>>, 
              TermExpr<PascalTokenPredicate<languages::pascal::PascalTokenId::MINUS>>
            >, 
            NonTermExpr<RuleId::kRule11>
          >
        >
      >
    >,

    // Rule #11 - term
    // term : factor ((MUL | INTEGER_DIV | FLOAT_DIV) factor)*
    Rule<NonTermTermNonTermSequenceProduction,
      SequenceExpr<
        NonTermExpr<RuleId::kRule12>,
        NMatchesOrMoreExpr<0,
          SequenceExpr<
            OrderedChoiceExpr<
              TermExpr<PascalTokenPredicate<languages::pascal::PascalTokenId::MULTIPLY>>, 
              TermExpr<PascalTokenPredicate<languages::pascal::PascalTokenId::INTEGER_DIV>>,
              TermExpr<PascalTokenPredicate<languages::pascal::PascalTokenId::FLOAT_DIV>>
            >, 
            NonTermExpr<RuleId::kRule12>
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
    OrderedChoiceRules< 
        Rule<TermNonTermProduction, 
          SequenceExpr<TermExpr<PascalTokenPredicate<languages::pascal::PascalTokenId::PLUS>>, NonTermExpr<RuleId::kRule12>>
        >,
        Rule<TermNonTermProduction, 
          SequenceExpr<TermExpr<PascalTokenPredicate<languages::pascal::PascalTokenId::MINUS>>, NonTermExpr<RuleId::kRule12>>
        >,
        Rule<TermProduction, 
          TermExpr<PascalTokenPredicate<languages::pascal::PascalTokenId::INTEGER_CONST>>
        >,
        Rule<TermProduction, 
          TermExpr<PascalTokenPredicate<languages::pascal::PascalTokenId::REAL_CONST>>
        >,
        Rule<BypassLastTermProduction,
          SequenceExpr<TermExpr<PascalTokenPredicate<languages::pascal::PascalTokenId::LPARENS>>, NonTermExpr<RuleId::kRule10>, TermExpr<PascalTokenPredicate<languages::pascal::PascalTokenId::RPARENS>>>
        >,
        Rule<BypassLastTermProduction, 
          NonTermExpr<RuleId::kRule13>
        >
    >,

    // Rule #13 - variable
    // variable: ID
    Rule<TermProduction, TermExpr<PascalTokenPredicate<languages::pascal::PascalTokenId::ID>>> 
  >;
}
// clang-format on

// TODO: Namespace languages/pascal einführen
using PGrammar = parser::PascalGrammar<std::shared_ptr<base::Ast<base::MakeShared, languages::pascal::PascalToken>>, languages::pascal::PascalLexer::iterator_type>;
using PascalParser = parser::Parser<PGrammar>;

#endif