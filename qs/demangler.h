// MIT License

// Copyright (c) 2025 Jose Sa

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.


#ifndef QS_DEMANGLER_H
#define QS_DEMANGLER_H

#include <qs/config.h>
#include <string>

#if (QS_GCC_VERSION || QS_CLANG_VERSION)
#include <cxxabi.h>
#include <memory>
#elif QS_MSVC_VERSION
#include <DbgHelp.h>
#include <windows.h>
#pragma comment(lib, "Dbghelp.lib")
#else
#endif

#if defined(__cpp_lib_string_view)
#include <string_view>
#endif

QS_NAMESPACE_BEGIN

template<typename T>
class demangler
{
public:
    static QS_CONSTEXPR14 std::string const& get()
    {
        if(type_name_.empty())
            set_default_();
        return type_name_;
    }

#if defined(__cpp_lib_string_view)
    static QS_CONSTEXPR17 void set(std::string_view const type_name) { type_name_ = type_name; }
#else
    template<size_t N>
    static QS_CONSTEXPR17 void set(char const (&type_name)[N])
    {
        type_name_.assign(type_name, N - 1);
    }
    static QS_CONSTEXPR17 void set(std::string const& type_name) { type_name_ = type_name; }
#endif

private:
    QS_INLINE_VAR static std::string type_name_ QS_INLINE_VAR_INIT({});

    static QS_CONSTEXPR17 void set_default_()
    {
#if QS_GCC_VERSION || QS_CLANG_VERSION
        int                   status = 0;
        size_t                len    = 0;
        std::unique_ptr<char> demangled;
        demangled.reset(abi::__cxa_demangle(typeid(T).name(), nullptr, &len, &status));
        if(status == 0)
            type_name_.assign(demangled.get(), len - 1);
        else
            type_name_ = typeid(T).name();

#else
        type_name_ = typeid(T).name();
#endif
    }
};

#if !defined(__cpp_inline_variables)
template<typename T>
std::string demangler<T>::type_name_{};
#endif


QS_NAMESPACE_END


#endif // QS_DEMANGLER_H
