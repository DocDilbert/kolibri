#ifndef KOLIBRI_SRC_PASCAL_PARSER_FACTORY_H_
#define KOLIBRI_SRC_PASCAL_PARSER_FACTORY_H_

#include "languages/ast.h"
#include "languages/ast_types.h"
#include "languages/i_ast_factory.h"
#include "languages/pascal/pascal_token.h"
#include "parser/i_parser_factory.h"

namespace languages {
namespace pascal {

class PascalParserFactory : public parser::IParserFactory<std::shared_ptr<Ast<MakeShared, PascalToken>>, PascalToken> {
 public:
  using nonterm_type = std::shared_ptr<Ast<MakeShared, PascalToken>>;
  using term_type = PascalToken;

  PascalParserFactory(IAstFactory<nonterm_type, term_type>& ast_factory) : ast_factory_(ast_factory) {}

  nonterm_type CreateNull() override { return ast_factory_.CreateNull(); }

  nonterm_type CreateEmpty(parser::RuleId rule_id) override { return ast_factory_.CreateNop(); }

  nonterm_type CreateTerm(parser::RuleId rule_id, term_type term) override {
    switch (rule_id) {
      case parser::RuleId::kRule4: {  // type_spec
        return ast_factory_.CreateRaw(term);
      }
      case parser::RuleId::kRule12: {  // factor
        switch (term.GetId()) {
          case PascalTokenId::kIntegerConst: {
            return ast_factory_.CreateConst(ConstType::kInteger, term);
          }
          case PascalTokenId::kRealConst: {
            return ast_factory_.CreateConst(ConstType::kReal, term);
          }
          default: {
            return ast_factory_.CreateNull();
          }
        }
      }
      case parser::RuleId::kRule13: {  // variable
        auto var_name = std::string(term.GetValue());
        return ast_factory_.CreateId(term);
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
        assert(nonterm->GetTypeId() == AstTypeId::kAstRawListType);
        auto& raw_list = dynamic_cast<AstRawList<MakeShared, term_type>&>(*nonterm);
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
        return ast_factory_.CreateUnaryOp(term, nonterm);
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
        assert(lhs->GetTypeId() == AstTypeId::kAstRawListType);
        auto& var_decl_list = dynamic_cast<AstRawList<MakeShared, term_type>&>(*lhs);

        return ast_factory_.CreateBlock(var_decl_list.Get(), rhs);
      }
      default: {
        return ast_factory_.CreateNull();
      }
    }
     // this should not happen
    return ast_factory_.CreateNull();
  }

  nonterm_type CreateNonTermTermNonTerm(parser::RuleId rule_id, nonterm_type lhs, term_type term, nonterm_type rhs) override {
    switch (rule_id) {
      case parser::RuleId::kRule8: {  // assignment_statement
        return ast_factory_.CreateBinaryOp(lhs, term, rhs);
      }

      case parser::RuleId::kRule10: {  // expr
        return ast_factory_.CreateBinaryOp(lhs, term, rhs);
      }

      case parser::RuleId::kRule11: {  // term
        return ast_factory_.CreateBinaryOp(lhs, term, rhs);
      }
      default: {
        return ast_factory_.CreateNull();
      }
    }
     // this should not happen
    return ast_factory_.CreateNull();
  }

  nonterm_type CreateNonTermList(parser::RuleId rule_id, std::vector<nonterm_type> nonterms) override {
    switch (rule_id) {
      case parser::RuleId::kRule2: {  // declarations
        std::vector<nonterm_type> var_decls;

        for (int i = 0; i < nonterms.size(); ++i) {
          auto nonterm = nonterms[i];
          if (nonterm->GetTypeId() == AstTypeId::kAstRawListType) {
            auto& var_decl_list = dynamic_cast<AstRawList<MakeShared, term_type>&>(*nonterm);
            auto var_decl_vector = var_decl_list.Get();
            var_decls.insert(var_decls.end(), var_decl_vector.begin(), var_decl_vector.end());
          }
        }
        return ast_factory_.CreateRawList(var_decls);
      }
      case parser::RuleId::kRule6: { // statement_list
        return ast_factory_.CreateRawList(nonterms);
      }

      default: {
        return ast_factory_.CreateNull();
      }
    }
     // this should not happen
    return ast_factory_.CreateNull();
  }

  nonterm_type CreateTermNonTermList(parser::RuleId rule_id, std::vector<term_type> terms, std::vector<nonterm_type> nonterms) override {
    std::vector<term_type> id_terms;

    for (int i = 0; i < terms.size(); ++i) {
      auto term = terms[i];
      if (term.GetId() == PascalTokenId::kId) {
        id_terms.push_back(term);
      }
    }

    assert(nonterms.size() == 1);
    assert(nonterms[0]->GetTypeId() == AstTypeId::kAstRawType);

    // extract from Raw node
    auto type_term = dynamic_cast<AstRaw<MakeShared, term_type>&>(*nonterms[0]).GetTerm();

    std::vector<nonterm_type> var_decls;
    for (int i = 0; i < id_terms.size(); ++i) {
      auto id = id_terms[i];
      var_decls.push_back(ast_factory_.CreateVariableDeclaration(id, type_term));
    }
     // this should not happen
    return ast_factory_.CreateRawList(var_decls);
  }

 private:
  IAstFactory<nonterm_type, term_type>& ast_factory_;
};

}  // namespace pascal
}  // namespace languages
#endif