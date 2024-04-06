#pragma once

#include <string>
#include <string_view>

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
