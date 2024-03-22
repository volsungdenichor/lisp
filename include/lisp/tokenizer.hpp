#pragma once

#include <string>
#include <vector>

namespace lisp
{

using token = std::string;

std::vector<token> tokenize(std::string text);

}  // namespace lisp
