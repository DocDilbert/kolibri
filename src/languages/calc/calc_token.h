#ifndef KOLIBRI_SRC_CALC_TOKEN_H_
#define KOLIBRI_SRC_CALC_TOKEN_H_

#include "base/token.h"

namespace languages {
namespace calc {

enum class CalcTokenId {
  kUnknown,  //
  kPlus,
  kMinus,
  kMultiply,
  kDiv,
  kInteger,
  kNullterm,
  kLParens,
  kRParens,
  kKeyword,
  kEndOfFile
};

class CalcTokenIdConverter {
 public:
  static constexpr const char* ToString(CalcTokenId id_) {
    switch (id_) {
      case CalcTokenId::kPlus:
        return "PLUS";
      case CalcTokenId::kMinus:
        return "MINUS";
      case CalcTokenId::kMultiply:
        return "MULTIPLY";
      case CalcTokenId::kDiv:
        return "DIVIDE";
      case CalcTokenId::kInteger:
        return "INTEGER";
      case CalcTokenId::kNullterm:
        return "NULLTERM";
      case CalcTokenId::kLParens:
        return "LPARENS";
      case CalcTokenId::kRParens:
        return "RPARENS";
      case CalcTokenId::kKeyword:
        return "KEYWORD";
      case CalcTokenId::kEndOfFile:
        return "ENDOFFILE";
      default:
        break;
    }
    return "UNKNOWN";
  }
};

using CalcToken = base::Token<languages::calc::CalcTokenId, languages::calc::CalcTokenIdConverter>;
using CalcTokenId = typename CalcToken::id_type;

}  // namespace calc
}  // namespace languages
#endif