#pragma once

#include <string>
#include <string_view>
#include <vector>

namespace lisp
{

using token = std::string;

std::vector<token> tokenize(std::string_view text);

}  // namespace lisp
