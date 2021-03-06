#ifndef KOLIBRI_SRC_CALC_PARSER_FACTORY_H_
#define KOLIBRI_SRC_CALC_PARSER_FACTORY_H_

#include "languages/ast.h"
#include "languages/ast_types.h"
#include "languages/i_ast_factory.h"
#include "languages/calc/calc_token.h"
#include "parser/i_parser_factory.h"

namespace languages {
namespace calc {

class CalcParserFactory : public parser::IParserFactory<std::shared_ptr<Ast<MakeShared, CalcToken>>, CalcToken> {
 public:
  using nonterm_type = std::shared_ptr<Ast<MakeShared, CalcToken>>;
  using term_type = CalcToken;

  CalcParserFactory(IAstFactory<nonterm_type, term_type>& ast_factory) : ast_factory_(ast_factory) {}

  nonterm_type CreateNull() override { return ast_factory_.CreateNull(); }
  nonterm_type CreateEmpty(parser::RuleId rule_id) override { return ast_factory_.CreateNop(); }

  nonterm_type CreateTerm(parser::RuleId rule_id, term_type term) override {
    switch (rule_id) {
      case parser::RuleId::kRule2: {
        return ast_factory_.CreateConst(ConstType::kInteger, term);
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
        return ast_factory_.CreateUnaryOp(term, nonterm);
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
        return ast_factory_.CreateBinaryOp(lhs, term, rhs);
      }
      case parser::RuleId::kRule1: {
        return ast_factory_.CreateBinaryOp(lhs, term, rhs);
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
  IAstFactory<nonterm_type, term_type>& ast_factory_;
};

}  // namespace calc
}  // namespace languages
#endif