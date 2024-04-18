#pragma once

#include <ostream>
#include <string>

#ifdef __GNUG__
#include <cxxabi.h>

#include <cstdlib>
#include <memory>

inline std::string demangle(const char* name)
{
    int status = -4;
    std::unique_ptr<char, void (*)(void*)> res{ abi::__cxa_demangle(name, NULL, NULL, &status), std::free };
    return (status == 0) ? res.get() : name;
}

#else

// does nothing if not g++
inline std::string demangle(const char* name)
{
    return name;
}

#endif

template <class T>
std::string_view type_name()
{
    static const std::string result = demangle(typeid(T).name());
    return result;
}

template <class T>
std::string_view type_name(const T&)
{
    return type_name<T>();
}

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
