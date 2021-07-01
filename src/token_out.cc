#include "token_out.h"

#include <utility>

using namespace std;

std::ostream& operator<<(std::ostream& os, const base::Token<languages::calc::CalcTokenId, languages::calc::CalcTokenIdConverter>& token) {
  auto value = token.GetValue();
  if (value.length() == 0) {
    os << "Token(" << token.GetStringId() << ")";
  } else {
    os << "Token(" << token.GetStringId() << ", " << value << ")";
  }
  return os;
}

std::ostream& operator<<(std::ostream& os, const base::Token<languages::pascal::PascalTokenId, languages::pascal::PascalTokenIdConverter>& token) {
  auto value = token.GetValue();
  if (value.length() == 0) {
    os << "Token(" << token.GetStringId() << ")";
  } else {
    os << "Token(" << token.GetStringId() << ", " << value << ")";
  }
  return os;
}