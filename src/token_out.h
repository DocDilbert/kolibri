#ifndef KOLIBRI_SRC_TOKEN_OUT_H_
#define KOLIBRI_SRC_TOKEN_OUT_H_

#include <iostream>
#include <string>
#include <string_view>

#include "base/token.h"
#include "languages/calc/calc_token.h"
#include "languages/pascal/pascal_token.h"

std::ostream& operator<<(std::ostream& os, const base::Token<languages::calc::CalcTokenId, languages::calc::CalcTokenIdConverter>& token);
std::ostream& operator<<(std::ostream& os, const base::Token<languages::pascal::PascalTokenId, languages::pascal::PascalTokenIdConverter>& token);

#endif