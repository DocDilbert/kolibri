#ifndef INTERTEST_SRC_CALC_TOKEN_H_
#define INTERTEST_SRC_CALC_TOKEN_H_

#include "base/token.h"

namespace languages {
namespace calc {

enum class CalcTokenId {
  UNKNOWN,  //
  PLUS,
  MINUS,
  MULTIPLY,
  DIVIDE,
  INTEGER,
  NULLTERM,
  LPARENS,
  RPARENS,
  KEYWORD,
  COMMENT,
  ENDOFFILE
};

class CalcTokenIdConverter {
 public:
  std::string ToString(CalcTokenId id_) const {
    switch (id_) {
      case CalcTokenId::PLUS:
        return "PLUS";
      case CalcTokenId::MINUS:
        return "MINUS";
      case CalcTokenId::MULTIPLY:
        return "MULTIPLY";
      case CalcTokenId::DIVIDE:
        return "DIVIDE";
      case CalcTokenId::INTEGER:
        return "INTEGER";
      case CalcTokenId::NULLTERM:
        return "NULLTERM";
      case CalcTokenId::LPARENS:
        return "LPARENS";
      case CalcTokenId::RPARENS:
        return "RPARENS";
      case CalcTokenId::KEYWORD:
        return "KEYWORD";
      case CalcTokenId::COMMENT:
        return "COMMENT";
      case CalcTokenId::ENDOFFILE:
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