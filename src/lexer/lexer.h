#ifndef KOLIBRI_SRC_LEXER_H_
#define KOLIBRI_SRC_LEXER_H_

#include <iterator>

namespace lexer {

template <typename Rules>
class Lexer {
 public:
  using value_type = typename Rules::value_type;
  
  class LexerIterator {
   public:
    using value_type = value_type;
    using difference_type = void;
    using pointer = value_type*;
    using reference = value_type&;
    using iterator_category = std::input_iterator_tag;

    LexerIterator(const char* it, const char* end) : it_(it), end_(end), last_token_() { last_token_ = GetNextToken(); }

    LexerIterator(const LexerIterator& other) : it_(other.it_), end_(other.end_), last_token_(other.last_token_) {}

    LexerIterator& operator++() {
      last_token_ = GetNextToken();
      return *this;
    }
    LexerIterator operator++(int) {
      LexerIterator tmp(*this);
      operator++();
      return tmp;
    }

    bool operator==(const LexerIterator& rhs) const { return (it_ == rhs.it_) && (end_ == rhs.end_) && (last_token_ == rhs.last_token_); }
    bool operator!=(const LexerIterator& rhs) const { return !(*this == rhs); }

    reference operator*() { return last_token_; }

   private:
    value_type GetNextToken() {
      auto token = rules_.Match(it_, end_);
      it_ = rules_.GetPosition();
      return token;
    }

    const char* it_;
    const char* end_;
    value_type last_token_;
    Rules rules_;
  };
  
  using iterator_type = LexerIterator;

  explicit Lexer(const char* begin, size_t len) : begin_(begin), end_(begin + len) {}
  ~Lexer() {}

  Lexer(Lexer const&) = delete;
  Lexer& operator=(Lexer const&) = delete;

  LexerIterator begin() { return LexerIterator(begin_, end_); }
  LexerIterator end() { return LexerIterator(end_, end_); }

 private:
  const char* begin_;
  const char* end_;
};

}  // namespace lexer

#endif