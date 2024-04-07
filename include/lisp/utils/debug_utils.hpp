#pragma once

#include <ostream>
#include <string>

struct code_location
{
    std::string file;
    int line;
    std::string function;

    friend std::ostream& operator<<(std::ostream& os, const code_location& item)
    {
        return os << item.file << ":" << item.line << " (" << item.function << ")";
    }
};

#define CODE_LOCATION code_location{ __FILE__, __LINE__, __FUNCTION__ };