#ifndef INTERTEST_SRC_PASCAL_TOKEN_H_
#define INTERTEST_SRC_PASCAL_TOKEN_H_

#include "base/token.h"

namespace languages {
namespace pascal {

enum class PascalTokenId {
  kUnknown,  //
  kPlus,
  kMinus,
  kMultiply,
  kIntegerDiv,
  kFloatDiv,
  kBegin,
  kEnd,
  kReal,
  kInteger,
  kRealConst,
  kIntegerConst,
  kId,
  kAssign,
  kSemi,
  kDot,
  kLParens,
  kRParens,
  kProgram,
  kVar,
  kColon,
  kComma,
  kEndOfFile
};

class PascalTokenIdConverter {
 public:
  static constexpr const char* ToString(PascalTokenId id_) {
    switch (id_) {
      case PascalTokenId::kPlus:
        return "PLUS";
      case PascalTokenId::kMinus:
        return "MINUS";
      case PascalTokenId::kMultiply:
        return "MULTIPLY";
      case PascalTokenId::kIntegerDiv:
        return "INTEGER_DIV";
      case PascalTokenId::kFloatDiv:
        return "FLOAT_DIV";
      case PascalTokenId::kBegin:
        return "BEGIN";
      case PascalTokenId::kEnd:
        return "END";
      case PascalTokenId::kReal:
        return "REAL";
      case PascalTokenId::kInteger:
        return "INTEGER";
      case PascalTokenId::kRealConst:
        return "REAL_CONST";
      case PascalTokenId::kIntegerConst:
        return "INTEGER_CONST";
      case PascalTokenId::kId:
        return "ID";
      case PascalTokenId::kAssign:
        return "ASSIGN";
      case PascalTokenId::kSemi:
        return "SEMI";
      case PascalTokenId::kDot:
        return "DOT";
      case PascalTokenId::kLParens:
        return "LPARENS";
      case PascalTokenId::kRParens:
        return "RPARENS";
      case PascalTokenId::kProgram:
        return "PROGRAM";
      case PascalTokenId::kVar:
        return "VAR";
      case PascalTokenId::kComma:
        return "COMMA";
      case PascalTokenId::kColon:
        return "COLON";
      case PascalTokenId::kEndOfFile:
        return "ENDOFFILE";
      default:
        break;
    }
    return "UNKNOWN";
  }
};

using PascalToken = base::Token<languages::pascal::PascalTokenId, languages::pascal::PascalTokenIdConverter>;
using PascalTokenId = typename PascalToken::id_type;

}  // namespace pascal
}  // namespace languages
#endif