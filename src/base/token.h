#ifndef INTERTEST_SRC_TOKEN_H_
#define INTERTEST_SRC_TOKEN_H_

#include <string>
#include <string_view>

namespace base {
// The Token class stores an id and the position of an token
// in a const char* buffer.
template <typename Id, typename IdConverter>
class Token {
 public:
  using id_type = Id;

  // construct token with Unknown id and no value
  explicit Token() : id_(id_type::UNKNOWN), value_() {}

  // construct token with id and no value
  explicit Token(id_type id) : id_(id), value_() {}

  // construct token with id. The value of the token is given by its begin ptr and its length len.
  explicit Token(id_type id, const char* begin, size_t len) : id_(id), value_(begin, len) {}

  // construct token with id. The value of the token is given by its begin pointer and its end pointer.
  explicit Token(id_type id, const char* begin, const char* end) : Token(id, begin, std::distance(begin, end)) {}

  Token(Token const& rhs) : id_(rhs.id_), value_(rhs.value_) {}
  Token& operator=(Token rhs) {
    std::swap(value_, rhs.value_);
    std::swap(id_, rhs.id_);
    return *this;
  }

  bool operator==(const Token& rhs) const { return (id_ == rhs.id_) && (value_ == rhs.value_); }
  bool operator!=(const Token& rhs) const { return !(*this == rhs); }

  id_type GetId() const { return id_; }

  std::string GetStringId() const {
    IdConverter conv_;
    return conv_.ToString(id_);
  }

  std::string_view GetValue() const { return value_; }

 private:
  //friend std::ostream& ::operator<<(std::ostream& os, const Token& token);

  id_type id_;
  std::string_view value_;
};
}  // namespace base
#endif