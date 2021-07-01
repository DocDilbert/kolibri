#ifndef KOLIBRI_SRC_I_PARSER_FACTORY_H_
#define KOLIBRI_SRC_I_PARSER_FACTORY_H_

#include <vector>

#include "parser/rule_id.h"

namespace parser {

template <typename TNonTerm, typename TTerm>
class IParserFactory {
 public:
  using nonterm_type = TNonTerm;
  using term_type = TTerm;

  virtual nonterm_type CreateNull() = 0;
  virtual nonterm_type CreateEmpty(RuleId rule_id) = 0;
  virtual nonterm_type CreateTerm(RuleId rule_id, term_type term) = 0;
  virtual nonterm_type CreateNonTerm(RuleId rule_id, nonterm_type nonterm) = 0;
  virtual nonterm_type CreateTermNonTerm(RuleId rule_id, term_type term, nonterm_type nonterm) = 0;
  virtual nonterm_type CreateNonTermNonTerm(RuleId rule_id, nonterm_type lhs, nonterm_type rhs) = 0;
  virtual nonterm_type CreateNonTermTermNonTerm(RuleId rule_id, nonterm_type lhs, term_type term, nonterm_type rhs) = 0;
  virtual nonterm_type CreateNonTermList(RuleId rule_id, std::vector<nonterm_type> statements) = 0;
  virtual nonterm_type CreateTermNonTermList(RuleId rule_id, std::vector<term_type> terms, std::vector<nonterm_type> nonterms) = 0;
};

}  // namespace parser
#endif