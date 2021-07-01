#ifndef KOLIBRI_SRC_PARSER_H_
#define KOLIBRI_SRC_PARSER_H_

#include "parser/i_parser_factory.h"
#include "parser/rule_id.h"

namespace parser {



template <typename Grammar>
class Parser {
 public:
  using nonterm_type = typename Grammar::nonterm_type;
  using term_type = typename Grammar::term_type;

  explicit Parser(IParserFactory<nonterm_type, term_type>& parser_factory) : parser_factory_(parser_factory) {}

  ~Parser() {}

  Parser(Parser const&) = delete;
  Parser& operator=(Parser const&) = delete;

  struct ExprResult {
    nonterm_type node;
    bool is_error;
    const char* error_msg;
  };

  template <typename Iterator>
  ExprResult Expr(Iterator begin, Iterator end) {
    auto it = begin;

    auto res = parser_grammar_.Match(parser_factory_, RuleId::kRule0, it, end);

    if (res.is_error) {
      return {res.node, true, res.msg};
    }
    if (!res.is_match) {
      return {parser_factory_.CreateNull(), true, "Error: Rule#0 doesnt't match"};
    }

    if (it == end) {
      return {res.node, false, res.msg};
    } else {
      return {parser_factory_.CreateNull(), true, "ERROR: Tokens left"};
    }
  }

 private:
  IParserFactory<nonterm_type, term_type>& parser_factory_;
  Grammar parser_grammar_;
};

}  // namespace parser

#endif