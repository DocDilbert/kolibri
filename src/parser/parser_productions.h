#ifndef KOLIBRI_SRC_PARSER_PRODUCTIONS_H_
#define KOLIBRI_SRC_PARSER_PRODUCTIONS_H_
#include <assert.h>

#include <memory>
#include <vector>

#include "parser/i_parser_factory.h"
#include "parser/rule_id.h"

namespace parser {

/// Produces a NOP in an ast.
template <typename TNonTerm, typename TTerm>
class EmptyProduction {
 public:
  using nonterm_type = TNonTerm;
  using term_type = TTerm;

  EmptyProduction(RuleId rule_id) : rule_id_(rule_id) {}

  void AddTerminal(term_type const& terminal) {
    // do nothing
  }
  void AddNonTerminal(nonterm_type const& nonterminal) {
    // do nothing
  }

  nonterm_type Create(IParserFactory<nonterm_type, term_type>& parser_factory) { return parser_factory.CreateEmpty(rule_id_); }

 private:
  RuleId rule_id_;
};

template <typename TNonTerm, typename TTerm>
class BypassLastTermProduction {
 public:
  using nonterm_type = TNonTerm;
  using term_type = TTerm;

  BypassLastTermProduction(RuleId rule_id) : nonterminal_() {}

  void AddTerminal(term_type const& terminal) {
    // do nothing here
  }

  void AddNonTerminal(nonterm_type const& nonterminal) { 
    nonterminal_ = nonterminal; }

  nonterm_type Create(IParserFactory<nonterm_type, term_type>& parser_factory) { return nonterminal_; }

 private:
  nonterm_type nonterminal_;
};

template <typename TNonTerm, typename TTerm>
class TermProduction {
 public:
  using nonterm_type = TNonTerm;
  using term_type = TTerm;
  TermProduction(RuleId rule_id) : rule_id_(rule_id) {}

  void AddTerminal(term_type const& terminal) { term0_ = terminal; }

  void AddNonTerminal(nonterm_type const& nonterminal) {
    // do nothing here
  }

  nonterm_type Create(IParserFactory<nonterm_type, term_type>& parser_factory) { return parser_factory.CreateTerm(rule_id_, term0_); }

 private:
  RuleId rule_id_;
  term_type term0_;
};

template <typename TNonTerm, typename TTerm>
class NonTermProduction {
 public:
  using nonterm_type = TNonTerm;
  using term_type = TTerm;
  NonTermProduction(RuleId rule_id) : rule_id_(rule_id) {}

  void AddTerminal(term_type const& terminal) {  }

  void AddNonTerminal(nonterm_type const& nonterminal) {
    nonterm0_ = nonterminal;
  }

  nonterm_type Create(IParserFactory<nonterm_type, term_type>& parser_factory) { 
    return parser_factory.CreateNonTerm(rule_id_, nonterm0_); }

 private:
  RuleId rule_id_;
  nonterm_type nonterm0_;
};


template <typename TNonTerm, typename TTerm>
class NonTermNonTermProduction {
 public:
  using nonterm_type = TNonTerm;
  using term_type = TTerm;

  NonTermNonTermProduction(RuleId rule_id) : rule_id_(rule_id), call_cnt_(0), lhs_(), rhs_() {}

  void AddTerminal(term_type const& terminal) {

   }

  void AddNonTerminal(nonterm_type const& nonterminal) { 
    switch (call_cnt_) {
      case 0:
        lhs_ = nonterminal;
        call_cnt_++;
        break;
      case 1:
        rhs_ = nonterminal;
        call_cnt_++;
        break;
      default:
        break;
    }
   }

  nonterm_type Create(IParserFactory<nonterm_type, term_type>& parser_factory) { return parser_factory.CreateNonTermNonTerm(rule_id_, lhs_, rhs_); }

 private:
  RuleId rule_id_;
  unsigned call_cnt_;
  nonterm_type lhs_;
  nonterm_type rhs_;
};

template <typename TNonTerm, typename TTerm>
class TermNonTermProduction {
 public:
  using nonterm_type = TNonTerm;
  using term_type = TTerm;

  TermNonTermProduction(RuleId rule_id) : nonterminal_(), rule_id_(rule_id) {}

  void AddTerminal(term_type const& terminal) { terminal_ = terminal; }

  void AddNonTerminal(nonterm_type const& nonterminal) { nonterminal_ = nonterminal; }

  nonterm_type Create(IParserFactory<nonterm_type, term_type>& parser_factory) { return parser_factory.CreateTermNonTerm(rule_id_, terminal_, nonterminal_); }

 private:
  RuleId rule_id_;
  term_type terminal_;
  nonterm_type nonterminal_;
};




template <typename TNonTerm, typename TTerm>
class NonTermTermNonTermProduction {
 public:
  using nonterm_type = TNonTerm;
  using term_type = TTerm;

  NonTermTermNonTermProduction(RuleId rule_id) : rule_id_(rule_id), call_cnt_(0) {}

  void AddTerminal(term_type const& terminal) { term_ = terminal; }

  void AddNonTerminal(nonterm_type const& nonterminal) {
    switch (call_cnt_) {
      case 0:
        lhs_ = nonterminal;
        call_cnt_++;
        break;
      case 1:
        rhs_ = nonterminal;
        call_cnt_++;
        break;
      default:
        break;
    }
  }

  nonterm_type Create(IParserFactory<nonterm_type, term_type>& parser_factory) {
    auto result = parser_factory.CreateNonTermTermNonTerm(rule_id_, lhs_, term_, rhs_);

    return result;
  }

 private:
  RuleId rule_id_;
  unsigned call_cnt_;
  nonterm_type lhs_;
  term_type term_;
  nonterm_type rhs_;
};

template <typename TNonTerm, typename TTerm>
class NonTermTermNonTermSequenceProduction {
 public:
  using nonterm_type = TNonTerm;
  using term_type = TTerm;

  NonTermTermNonTermSequenceProduction(RuleId rule_id) : rule_id_(rule_id), nonterms_() {}

  void AddTerminal(term_type const& terminal) { terms_.push_back(terminal); }

  void AddNonTerminal(nonterm_type const& nonterminal) { nonterms_.push_back(nonterminal); }

  nonterm_type Create(IParserFactory<nonterm_type, term_type>& parser_factory) {
    if (nonterms_.size() == 0) {
      return parser_factory.CreateNull();
    } else if (nonterms_.size() == 1) {
      return nonterms_[0];
    } else {
      auto lhs = nonterms_[0];
      auto rhs = nonterms_[1];
      auto term = terms_[0];
      auto result = parser_factory.CreateNonTermTermNonTerm(rule_id_, lhs, term, rhs);
      for (int i = 2; i < nonterms_.size(); i++) {
        lhs = result;
        rhs = nonterms_[i];
        term = terms_[i - 1];
        result = parser_factory.CreateNonTermTermNonTerm(rule_id_, lhs, term, rhs);
      }

      return result;
    }
  }

 private:
  RuleId rule_id_;
  std::vector<nonterm_type> nonterms_;
  std::vector<term_type> terms_;
};

template <typename TNonTerm, typename TTerm>
class NonTermListProduction {
 public:
  using nonterm_type = TNonTerm;
  using term_type = TTerm;

  NonTermListProduction(RuleId rule_id) : rule_id_(rule_id), nonterms_() {}

  void AddTerminal(term_type const& terminal) {
    // do nothing
  }

  void AddNonTerminal(nonterm_type const& nonterminal) { nonterms_.push_back(nonterminal); }

  nonterm_type Create(IParserFactory<nonterm_type, term_type>& parser_factory) { return parser_factory.CreateNonTermList(rule_id_, nonterms_); }

 private:
  RuleId rule_id_;
  std::vector<nonterm_type> nonterms_;
};

template <typename TNonTerm, typename TTerm>
class TermNonTermListProduction {
 public:
  using nonterm_type = TNonTerm;
  using term_type = TTerm;

  TermNonTermListProduction(RuleId rule_id) : rule_id_(rule_id), nonterms_() {}

  void AddTerminal(term_type const& terminal) {
    terms_.push_back(terminal);
  }

  void AddNonTerminal(nonterm_type const& nonterminal) { nonterms_.push_back(nonterminal); }

  nonterm_type Create(IParserFactory<nonterm_type, term_type>& parser_factory) { return parser_factory.CreateTermNonTermList(rule_id_, terms_, nonterms_); }

 private:
  RuleId rule_id_;
  std::vector<term_type> terms_;
  std::vector<nonterm_type> nonterms_;
};

}  // namespace parser

#endif
