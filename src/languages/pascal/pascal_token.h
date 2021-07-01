#ifndef INTERTEST_SRC_PASCAL_TOKEN_H_
#define INTERTEST_SRC_PASCAL_TOKEN_H_

#include "base/token.h"

namespace languages {
namespace pascal {

enum class PascalTokenId {
  kUnknown,  //
  PLUS,
  MINUS,
  MULTIPLY,
  INTEGER_DIV,
  FLOAT_DIV,
  BEGIN,
  END,
  REAL,
  INTEGER,
  REAL_CONST,
  INTEGER_CONST,
  ID,
  ASSIGN,
  SEMI,
  DOT,
  LPARENS,
  RPARENS,
  PROGRAM,
  VAR,
  COLON,
  COMMA,
  ENDOFFILE
};

class PascalTokenIdConverter {
 public:
  static constexpr const char* ToString(PascalTokenId id_) {
    switch (id_) {
      case PascalTokenId::PLUS:
        return "PLUS";
      case PascalTokenId::MINUS:
        return "MINUS";
      case PascalTokenId::MULTIPLY:
        return "MULTIPLY";
      case PascalTokenId::INTEGER_DIV:
        return "INTEGER_DIV";
      case PascalTokenId::FLOAT_DIV:
        return "FLOAT_DIV";
      case PascalTokenId::BEGIN:
        return "BEGIN";
      case PascalTokenId::END:
        return "END";
      case PascalTokenId::REAL:
        return "REAL";
      case PascalTokenId::INTEGER:
        return "INTEGER";
      case PascalTokenId::REAL_CONST:
        return "REAL_CONST";
      case PascalTokenId::INTEGER_CONST:
        return "INTEGER_CONST";
      case PascalTokenId::ID:
        return "ID";
      case PascalTokenId::ASSIGN:
        return "ASSIGN";
      case PascalTokenId::SEMI:
        return "SEMI";
      case PascalTokenId::DOT:
        return "DOT";
      case PascalTokenId::LPARENS:
        return "LPARENS";
      case PascalTokenId::RPARENS:
        return "RPARENS";
      case PascalTokenId::PROGRAM:
        return "PROGRAM";
      case PascalTokenId::VAR:
        return "VAR";
      case PascalTokenId::COMMA:
        return "COMMA";
      case PascalTokenId::COLON:
        return "COLON";
      case PascalTokenId::ENDOFFILE:
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