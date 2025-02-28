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


#ifndef QS_LIFECYCLE_TRACKER_SINGLE_HEADER_H
#define QS_LIFECYCLE_TRACKER_SINGLE_HEADER_H


#include <array>
#include <atomic>
#include <cstddef> // requires for _HAS_EXCEPTIONS on MSVC
#include <cstdio>
#include <exception>
#include <new> // requires for std::hardware_destructive_interference_size
#include <string>
#include <tuple>
#include <type_traits>

#if defined(__cpp_lib_string_view)
#include <string_view>
#endif


#define QS_NAMESPACE qs

#define QS_NAMESPACE_BEGIN                                                                         \
    namespace QS_NAMESPACE                                                                         \
    {
#define QS_NAMESPACE_END }

// Stringify
#define QS_STRINGIFY(v) #v
// Concatenation
#define QS_CONCAT(A, B) A##B

// #define QS_COMPILER_VERSION_CONCAT(major, minor, patch) (10 * (10 * (major) + (minor)) + (patch))
#define QS_COMPILER_VERSION_CONCAT(major, minor, patch) ((100 * (major)) + (minor))
// Define a in between macro
#define QS_IN_BETWEEN(x, a, b) ((a) <= (x) && (x) < (b))

// Detect compiler versions.
#if defined(__clang__) && !defined(__ibmxl__)
#define QS_CLANG_VERSION                                                                           \
    QS_COMPILER_VERSION_CONCAT(__clang_major__, __clang_minor__, __clang_patchlevel__)
#else
#define QS_CLANG_VERSION 0
#endif
#if defined(__GNUC__) && !defined(__clang__) && !defined(__INTEL_COMPILER)
#define QS_GCC_VERSION QS_COMPILER_VERSION_CONCAT(__GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__)
#else
#define QS_GCC_VERSION 0
#endif
#if defined(__ICL)
#define QS_ICC_VERSION __ICL
#elif defined(__INTEL_COMPILER)
#define QS_ICC_VERSION __INTEL_COMPILER
#else
#define QS_ICC_VERSION 0
#endif
#if defined(_MSC_VER)
#define QS_MSVC_VERSION _MSC_VER
#else
#define QS_MSVC_VERSION 0
#endif

// Detect CPU Families
// Detect PowerPC (32-bit & 64-bit)
#if defined(__powerpc__) || defined(__PPC__) || defined(__ppc__)
#define QS_POWERPC 1
#else
#define QS_POWERPC 0
#endif
#if defined(__powerpc64__) || defined(__PPC64__) || defined(__ppc64__)
#define QS_POWERPC64 1
#else
#define QS_POWERPC64 0
#endif
// Detect ARM (32-bit & 64-bit)
#if defined(__arm__) || defined(_M_ARM)
#define QS_ARM 1
#else
#define QS_ARM 0
#endif
#if defined(__aarch64__) || defined(_M_ARM64)
#define QS_ARM64 1
#else
#define QS_ARM64 0
#endif
#if defined(__APPLE__) && QS_ARM64
#define QS_ARM64_APPLE 1
#else
#define QS_ARM64_APPLE 0
#endif
// Detect x86 (32-bit & 64-bit)
#if defined(__x86_64__) || defined(_M_X64)
#define QS_X86_64 1
#else
#define QS_X86_64 0
#endif
#if defined(__i386__) || defined(_M_IX86)
#define QS_X86 1
#else
#define QS_X86 0
#endif

// Detect standard library versions.
#ifdef _GLIBCXX_RELEASE
#define QS_GLIBCXX_VERSION _GLIBCXX_RELEASE
#else
#define QS_GLIBCXX_VERSION 0
#endif
#ifdef _LIBCPP_VERSION
#define QS_LIBCPP_VERSION _LIBCPP_VERSION
#else
#define QS_LIBCPP_VERSION 0
#endif

#ifdef _MSVC_LANG
#define QS_CPLUSPLUS _MSVC_LANG
#else
#define QS_CPLUSPLUS __cplusplus
#endif

#if QS_CPLUSPLUS >= 202302L
#define QS_STD_VERSION 23
#elif QS_CPLUSPLUS >= 202002L
#define QS_STD_VERSION 20
#elif QS_CPLUSPLUS >= 201703L
#define QS_STD_VERSION 17
#elif QS_CPLUSPLUS >= 201402L
#define QS_STD_VERSION 14
#else
#define QS_STD_VERSION 11
#endif

// Detect __has_*.
#ifdef __has_builtin
#define QS_HAS_BUILTIN(x) __has_builtin(x)
#else
#define QS_HAS_BUILTIN(x) 0
#endif
#ifdef __has_feature
#define QS_HAS_FEATURE(x) __has_feature(x)
#else
#define QS_HAS_FEATURE(x) 0
#endif
#ifdef __has_include
#define QS_HAS_INCLUDE(x) __has_include(x)
#else
#define QS_HAS_INCLUDE(x) 0
#endif
#ifdef __has_cpp_attribute
#define QS_HAS_CPP_ATTRIBUTE(x) __has_cpp_attribute(x)
#else
#define QS_HAS_CPP_ATTRIBUTE(x) 0
#endif
#define QS_HAS_CPP14_ATTRIBUTE(attribute) (QS_STD_VER >= 14 && QS_HAS_CPP_ATTRIBUTE(attribute))
#define QS_HAS_CPP17_ATTRIBUTE(attribute) (QS_STD_VER >= 17 && QS_HAS_CPP_ATTRIBUTE(attribute))


// Detect C++11 constexpr.
#ifdef QS_USE_CONSTEXPR11
// Use the provided definition.
#elif QS_CLANG_VERSION && QS_HAS_FEATURE(cxx_constexpr)
#define QS_USE_CONSTEXPR11 1
#elif QS_CLANG_VERSION >= 406
#define QS_USE_CONSTEXPR11 1
#elif QS_MSVC_VERSION >= 1900 // MSVC 2015+
#define QS_USE_CONSTEXPR11 1
#else
#define QS_USE_CONSTEXPR11 0
#endif
#if QS_USE_CONSTEXPR11
#define QS_CONSTEXPR11 constexpr
#else
#define QS_CONSTEXPR11
#endif

// Detect C++14 relaxed constexpr.
#ifdef QS_USE_CONSTEXPR14
// Use the provided definition.
#elif QS_GCC_VERSION >= 600 && QS_CPLUSPLUS >= 201402L
#define QS_USE_CONSTEXPR14 1
#elif QS_ICC_VERSION
#define QS_USE_CONSTEXPR14 0
#elif QS_HAS_FEATURE(cxx_relaxed_constexpr) || QS_MSVC_VERSION >= 1912
#define QS_USE_CONSTEXPR14 1
#else
#define QS_USE_CONSTEXPR14 0
#endif
#if QS_USE_CONSTEXPR14
#define QS_CONSTEXPR14 constexpr
#else
#define QS_CONSTEXPR14
#endif

// Detect C++17 constexpr support
#ifdef QS_USE_CONSTEXPR17
// Use the provided definition.
#elif QS_GCC_VERSION >= 700 && QS_CPLUSPLUS >= 201703L
#define QS_USE_CONSTEXPR17 1
#elif QS_ICC_VERSION
#define QS_USE_CONSTEXPR17 0
#elif QS_HAS_FEATURE(cxx_constexpr_if) || QS_MSVC_VERSION >= 1914
#define QS_USE_CONSTEXPR17 1
#else
#define QS_USE_CONSTEXPR17 0
#endif
#if QS_USE_CONSTEXPR17
#define QS_CONSTEXPR17 constexpr
#else
#define QS_CONSTEXPR17
#endif

// Detect consteval, C++20 constexpr extensions and std::is_constant_evaluated.
#if !defined(__cpp_lib_is_constant_evaluated)
#define QS_USE_CONSTEVAL 0
#elif QS_CPLUSPLUS < 201709L
#define QS_USE_CONSTEVAL 0
#elif QS_GLIBCXX_RELEASE && QS_GLIBCXX_RELEASE < 10
#define QS_USE_CONSTEVAL 0
#elif QS_LIBCPP_VERSION && QS_LIBCPP_VERSION < 10000
#define QS_USE_CONSTEVAL 0
#elif defined(__apple_build_version__) && __apple_build_version__ < 14000029L
#define QS_USE_CONSTEVAL 0 // consteval is broken in Apple clang < 14.
#elif QS_MSVC_VERSION && QS_MSVC_VERSION < 1929
#define QS_USE_CONSTEVAL 0 // consteval is broken in MSVC VS2019 < 16.10.
#elif defined(__cpp_consteval)
#define QS_USE_CONSTEVAL 1
#elif QS_GCC_VERSION >= 1002 || QS_CLANG_VERSION >= 1101
#define QS_USE_CONSTEVAL 1
#else
#define QS_USE_CONSTEVAL 0
#endif
#if QS_USE_CONSTEVAL
#define QS_CONSTEVAL consteval
#define QS_CONSTEXPR20 constexpr
#else
#define QS_CONSTEVAL
#define QS_CONSTEXPR20
#endif


#ifdef QS_EXCEPTIONS
// Use the provided definition.
#elif defined(__cpp_exceptions) || defined(__EXCEPTIONS) || _HAS_EXCEPTIONS
#define QS_EXCEPTIONS 1
#else
#define QS_EXCEPTIONS 0
#endif
#if QS_EXCEPTIONS
#define QS_TRY try
#define QS_CATCH(x) catch(x)
#else
#define QS_TRY if(true)
#define QS_CATCH(x) if(false)
#endif


#if QS_HAS_CPP17_ATTRIBUTE(fallthrough)
#define QS_FALLTHROUGH [[fallthrough]]
#elif QS_CLANG_VERSION
#define QS_FALLTHROUGH [[clang::fallthrough]]
#elif QS_GCC_VERSION >= 700 && (!defined(__EDG_VERSION__) || __EDG_VERSION__ >= 520)
#define QS_FALLTHROUGH [[gnu::fallthrough]]
#else
#define QS_FALLTHROUGH
#endif

// Disable [[noreturn]] on MSVC/NVCC because of bogus unreachable code warnings.
#if QS_HAS_CPP_ATTRIBUTE(noreturn) && !QS_MSVC_VERSION && !defined(__NVCC__)
#define QS_NORETURN [[noreturn]]
#else
#define QS_NORETURN
#endif

#ifndef QS_NODISCARD
#if QS_HAS_CPP17_ATTRIBUTE(nodiscard)
#define QS_NODISCARD [[nodiscard]]
#else
#define QS_NODISCARD
#endif
#endif

#ifdef QS_DEPRECATED
// Use the provided definition.
#elif QS_HAS_CPP14_ATTRIBUTE(deprecated)
#define QS_DEPRECATED [[deprecated]]
#define QS_DEPRECATED_F(msg) [[deprecated(msg)]]
#else
#define QS_DEPRECATED
#define QS_DEPRECATED_F(...)
#endif

#ifdef QS_INLINE
// Use the provided definition.
#elif QS_GCC_VERSION || QS_CLANG_VERSION
#define QS_ALWAYS_INLINE inline __attribute__((always_inline))
#else
#define QS_ALWAYS_INLINE inline
#endif
// A version of QS_INLINE to prevent code bloat in debug mode.
#ifdef NDEBUG
#define QS_INLINE QS_ALWAYS_INLINE
#else
#define QS_INLINE inline
#endif

#if QS_GCC_VERSION || QS_CLANG_VERSION
#define QS_VISIBILITY(value) __attribute__((visibility(value)))
#else
#define QS_VISIBILITY(value)
#endif

#ifdef QS_INLINE_VAR
// Use the provided definition
#elif defined(__cpp_inline_variables)
#define QS_INLINE_VAR inline
#define QS_INLINE_VAR_INIT(...) __VA_ARGS__
#else
#define QS_INLINE_VAR
#define QS_INLINE_VAR_INIT(...)
#endif


#ifdef QS_CACHELINE_SIZE
// Use the provided definition
#elif defined(__cpp_lib_hardware_interference_size)
#define QS_CACHELINE_SIZE std::hardware_destructive_interference_size
#elif (QS_X86_64 || QS_X86)
#define QS_CACHELINE_SIZE 64 // x86 and x86_64 CPUs (Intel & AMD)
#elif QS_ARM64_APPLE
#define QS_CACHELINE_SIZE 128 // Apple Sillicon chips
#elif (QS_ARM64 || QS_ARM)
#define QS_CACHELINE_SIZE 64 // ARM 64-bit (except Apple) and 32-bit (default assumption)
#elif QS_POWERPC64
#define QS_CACHELINE_SIZE 128 // PowerPC64 (IBM POWER9/10)
#elif QS_POWERPC
#define QS_CACHELINE_SIZE 64 // PowerPC 32-bit (default)
#else
#define QS_CACHELINE_SIZE 64 // Default assumption
#endif


QS_NAMESPACE_BEGIN

namespace intl
{
    template<class... Args>
    QS_CONSTEXPR14 void ignore_unused(Args&&...)
    {}
} // namespace intl

QS_NAMESPACE_END


// -----------------------------------------------------------------------------
// demangler
// -----------------------------------------------------------------------------

// demangler includes
#if (QS_GCC_VERSION || QS_CLANG_VERSION)
#include <cxxabi.h>
#include <memory>
#elif QS_MSVC_VERSION
#include <DbgHelp.h>
#include <windows.h>
#pragma comment(lib, "Dbghelp.lib")
#else
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


// -----------------------------------------------------------------------------
// lifecycle_tracker
// -----------------------------------------------------------------------------


// Check for fmt library support
#ifdef QS_LIFECYCLE_TRACKER_WITH_FMTLIB
// user provided option
#elif QS_HAS_INCLUDE("fmt/core.h")
#define QS_LIFECYCLE_TRACKER_WITH_FMTLIB 1
#else
#define QS_LIFECYCLE_TRACKER_WITH_FMTLIB 0
#endif

// Define logging macros based on available libraries
#if QS_LIFECYCLE_TRACKER_WITH_FMTLIB
#include <fmt/core.h>
#include <fmt/format.h>
#define QS_LIFECYCLE_LOGGER_PRINT(...) fmt::print(__VA_ARGS__)
#define QS_LIFECYCLE_LOGGER_STRING_ARG(x) x
#elif defined(__cpp_lib_print)
#include <format>
#include <print>
#define QS_LIFECYCLE_LOGGER_PRINT(...) std::print(__VA_ARGS__)
#define QS_LIFECYCLE_LOGGER_STRING_ARG(x) x
#else
#define QS_LIFECYCLE_LOGGER_PRINT(...) std::printf(__VA_ARGS__)
#define QS_LIFECYCLE_LOGGER_STRING_ARG(x) static_cast<int>(x.size()), x.data()
#endif


QS_NAMESPACE_BEGIN

// Enum to represent different lifecycle events
enum class lifecycle_event
{
    Constructor     = 0,
    CopyConstructor = 1,
    MoveConstructor = 2,
    CopyAssignment  = 3,
    MoveAssignment  = 4,
    Destructor      = 5
};

// Struct to hold lifecycle counters
struct lifecycle_counters
{
    size_t constructor;
    size_t copy_constructor;
    size_t move_constructor;
    size_t copy_assignment;
    size_t move_assignment;
    size_t destructor;

    // Methods to calculate total constructed, assigned, and alive objects
    constexpr size_t total_constructed() const noexcept
    {
        return constructor + copy_constructor + move_constructor;
    }
    constexpr size_t total_assigned() const noexcept { return copy_assignment + move_assignment; }
    constexpr ptrdiff_t alive() const noexcept
    {
        return static_cast<ptrdiff_t>(total_constructed()) - static_cast<ptrdiff_t>(destructor);
    }

    // Equality operator for lifecycle_counters
    constexpr bool operator==(lifecycle_counters const& rhs) const noexcept
    {
        return constructor == rhs.constructor && copy_constructor == rhs.copy_constructor &&
               move_constructor == rhs.move_constructor && copy_assignment == rhs.copy_assignment &&
               move_assignment == rhs.move_assignment && destructor == rhs.destructor;
    }
};


// clang-format off
//
// The qs::lifecycle_logger struct is a template that provides logging functionality for lifecycle events
// of a given type T and a unique identifier Uuid to distinguish between different lifecycle_trackers.
// The default logger is qs::lifecycle_default_logger which should not be modified.
// The user can provide a custom logger by template specialization of qs::lifecycle_logger.
// NOTE: The logger is not thread-safe for both qs::lifecycle_tracker and qs::lifecycle_tracker_mt.
//
//
// template<class T, size_t Uuid = 0>
// struct lifecycle_logger
// {
//     using value_type      = T; // The type being tracked
//     using reference       = value_type&; // Reference to the type
//     using const_reference = value_type const&; // Const reference to the type
//     using pointer         = value_type*; // Pointer to the type
//     using const_pointer   = value_type const*; // Const pointer to the type

//     // Logs a specific lifecycle event for the given type instance
//     template<lifecycle_event Cnt>
//     QS_CONSTEXPR17 void log_event(const_reference self, std::string const& type_name) const;

//     // Prints the current lifecycle counters for the given type
//     QS_CONSTEXPR17 void print_counters(lifecycle_counters const& cnts,
//                                        std::string const&        type_name) const;
// };
//
// clang-format on


// Default logger for lifecycle events
template<class T, size_t Uuid = 0>
struct lifecycle_default_logger
{
    using value_type      = T;
    using reference       = value_type&;
    using const_reference = value_type const&;
    using pointer         = value_type*;
    using const_pointer   = value_type const*;

    // Log lifecycle event
    template<lifecycle_event Cnt>
    QS_CONSTEXPR17 void log_event(const_reference self, std::string const& type_name) const
    {
        intl::ignore_unused(self);
        QS_LIFECYCLE_LOGGER_PRINT(log_event_format<Cnt>(),
                                  QS_LIFECYCLE_LOGGER_STRING_ARG(type_name),
                                  QS_LIFECYCLE_LOGGER_STRING_ARG(type_name));
    }

    // Print lifecycle counters
    QS_CONSTEXPR17 void print_counters(lifecycle_counters const& cnts,
                                       std::string const&        type_name) const
    {
        QS_LIFECYCLE_LOGGER_PRINT(log_counters_format(), QS_LIFECYCLE_LOGGER_STRING_ARG(type_name),
                                  Uuid, cnts.total_constructed(), cnts.constructor,
                                  cnts.copy_constructor, cnts.move_constructor,
                                  cnts.total_assigned(), cnts.copy_assignment, cnts.move_assignment,
                                  cnts.destructor, cnts.alive());
    }

protected:
    // Get format string for logging events
    template<lifecycle_event Cnt>
    QS_INLINE static QS_CONSTEXPR14 char const* log_event_format()
    {
        return event_fmt_map_[static_cast<size_t>(Cnt)];
    }

    // Get format string for logging counters
    QS_INLINE static QS_CONSTEXPR14 char const* log_counters_format() { return counter_fmt_; }

private:
    // Format strings for different logging libraries
#if QS_LIFECYCLE_TRACKER_WITH_FMTLIB || defined(__cpp_lib_print)
    QS_INLINE_VAR static constexpr std::array<char const*, 6> event_fmt_map_{
        "{}(...)", "{}({} const&)", "{}({}&&)", "=({} const&)", "=({}&&)", "~{}()"};
    QS_INLINE_VAR static constexpr char const* counter_fmt_ =
        "Lifecycle tracker [type: {}, uuid: {}]\n"
        " * constructor (ctor/copy/move) : {:>5} ({}/{}/{})\n"
        " * assign (copy/move)           : {:>5} ({}/{})\n"
        " * destructor (alive)           : {:>5} ({})\n";
#else
    QS_INLINE_VAR static constexpr std::array<char const*, 6> event_fmt_map_{
        "%.*s(...)", "%.*s(%.*s const&)", "%.*s(%.*s&&)", "=(%.*s const&)", "=(%.*s&&)", "~%.*s()"};
    QS_INLINE_VAR static constexpr char const* counter_fmt_ =
        "Lifecycle tracker [type: %.*s, uuid: %zu]\n"
        " * constructor (ctor/copy/move) : %5zu (%zu/%zu/%zu)\n"
        " * assign (copy/move)           : %5zu (%zu/%zu)\n"
        " * destructor (alive)           : %5zu (%td)\n";
#endif
};

#if !defined(__cpp_inline_variables)
template<class T, size_t Uuid>
constexpr std::array<char const*, 6> lifecycle_default_logger<T, Uuid>::event_fmt_map_;
template<class T, size_t Uuid>
constexpr char const* lifecycle_default_logger<T, Uuid>::counter_fmt_;
#endif


// Logger for lifecycle events
template<class T, size_t Uuid = 0>
struct lifecycle_logger : lifecycle_default_logger<T, Uuid>
{};


// Namespace for internal implementation details
namespace intl
{
    // Base class for tracking lifecycle events
    template<class Derived, class T, size_t Uuid>
    class lifecycle_tracker_base
    {
        static_assert(std::is_class<T>::value, "T must be a class type");

    public:
        // Constructor
        QS_CONSTEXPR20 lifecycle_tracker_base()
        {
            log_and_increment<lifecycle_event::Constructor>();
        }

        // Copy constructor
        QS_CONSTEXPR20 lifecycle_tracker_base(lifecycle_tracker_base const&)
        {
            log_and_increment<lifecycle_event::CopyConstructor>();
        }

        // Copy assignment operator
        QS_CONSTEXPR20 lifecycle_tracker_base& operator=(lifecycle_tracker_base const& other)
        {
            if(this != std::addressof(other))
                log_and_increment<lifecycle_event::CopyAssignment>();
            return *this;
        }

        // Move constructor
        QS_CONSTEXPR20 lifecycle_tracker_base(lifecycle_tracker_base&&)
        {
            log_and_increment<lifecycle_event::MoveConstructor>();
        }

        // Move assignment operator
        QS_CONSTEXPR20 lifecycle_tracker_base& operator=(lifecycle_tracker_base&& other)
        {
            if(this != std::addressof(other))
                log_and_increment<lifecycle_event::MoveAssignment>();
            return *this;
        }

        // Destructor
        QS_CONSTEXPR20 ~lifecycle_tracker_base()
        {
            log_and_increment<lifecycle_event::Destructor>();
        }

        // Reset lifecycle counters
        static QS_CONSTEXPR17 void reset_counters() { counters_ = lifecycle_counters{}; }

        // Get lifecycle counters
        static QS_CONSTEXPR14 lifecycle_counters const& get_counters() { return counters_; }

        // Print lifecycle counters
        static QS_CONSTEXPR14 lifecycle_counters const& print_counters()
        {
            lifecycle_counters const& cnts = get_counters();
            logger_.print_counters(cnts, get_type_name());
            return cnts;
        }

#if defined(__cpp_lib_string_view)
        // Set type name using string_view
        static QS_CONSTEXPR17 void set_type_name(std::string_view const type_name)
        {
            type_name_ = type_name;
        }
#else
        // Set type name using char array
        template<size_t N>
        static QS_CONSTEXPR17 void set_type_name(char const (&type_name)[N])
        {
            type_name_.assign(type_name, N - 1);
        }
        // Set type name using string
        static QS_CONSTEXPR17 void set_type_name(std::string const& type_name)
        {
            type_name_ = type_name;
        }
#endif

        // Get type name
        static QS_CONSTEXPR14 std::string const& get_type_name()
        {
            if(type_name_.empty())
                set_type_name(demangler<T>::get());
            return type_name_;
        }

    protected:
        // Get pointer to derived class
        QS_CONSTEXPR14 Derived*       self() noexcept { return static_cast<Derived*>(this); }
        QS_CONSTEXPR14 Derived const* self() const noexcept
        {
            return static_cast<Derived const*>(this);
        }

    private:
        // Static variables for counters, type name, and logger
        QS_INLINE_VAR static lifecycle_counters counters_      QS_INLINE_VAR_INIT({});
        QS_INLINE_VAR static std::string type_name_            QS_INLINE_VAR_INIT({});
        QS_INLINE_VAR static lifecycle_logger<T, Uuid> logger_ QS_INLINE_VAR_INIT({});

        // Get reference to specific counter
        template<lifecycle_event Cnt>
        static QS_CONSTEXPR11 size_t& get_counter() noexcept
        {
            return std::get<static_cast<size_t>(Cnt)>(std::tie(
                counters_.constructor, counters_.copy_constructor, counters_.move_constructor,
                counters_.copy_assignment, counters_.move_assignment, counters_.destructor));
        }

        // Log event and increment counter
        template<lifecycle_event Cnt>
        QS_CONSTEXPR17 void log_and_increment() const
        {
            ++get_counter<Cnt>();
            logger_.template log_event<Cnt>(*static_cast<T const*>(self()), get_type_name());
        }
    };

#if !defined(__cpp_inline_variables)
    template<class Derived, class T, size_t Uuid>
    lifecycle_counters lifecycle_tracker_base<Derived, T, Uuid>::counters_{};
    template<class Derived, class T, size_t Uuid>
    std::string lifecycle_tracker_base<Derived, T, Uuid>::type_name_{};
    template<class Derived, class T, size_t Uuid>
    lifecycle_logger<T, Uuid> lifecycle_tracker_base<Derived, T, Uuid>::logger_{};
#endif

    // Base class for tracking lifecycle events with multi-threading support
    template<class Derived, class T, size_t Uuid>
    class lifecycle_tracker_mt_base
    {
        static_assert(std::is_class<T>::value, "T must be a class type");

    public:
        // Constructor
        QS_CONSTEXPR20 lifecycle_tracker_mt_base()
        {
            log_and_increment<lifecycle_event::Constructor>();
        }

        // Copy constructor
        QS_CONSTEXPR20 lifecycle_tracker_mt_base(lifecycle_tracker_mt_base const&)
        {
            log_and_increment<lifecycle_event::CopyConstructor>();
        }

        // Copy assignment operator
        QS_CONSTEXPR20 lifecycle_tracker_mt_base& operator=(lifecycle_tracker_mt_base const& other)
        {
            if(this != std::addressof(other))
                log_and_increment<lifecycle_event::CopyAssignment>();
            return *this;
        }

        // Move constructor
        QS_CONSTEXPR20 lifecycle_tracker_mt_base(lifecycle_tracker_mt_base&&) noexcept
        {
            log_and_increment<lifecycle_event::MoveConstructor>();
        }

        // Move assignment operator
        QS_CONSTEXPR20 lifecycle_tracker_mt_base&
        operator=(lifecycle_tracker_mt_base&& other) noexcept
        {
            if(this != std::addressof(other))
                log_and_increment<lifecycle_event::MoveAssignment>();
            return *this;
        }

        // Destructor
        QS_CONSTEXPR20 ~lifecycle_tracker_mt_base()
        {
            log_and_increment<lifecycle_event::Destructor>();
        }

        // Reset lifecycle counters
        static QS_CONSTEXPR17 void reset_counters()
        {
            // ensure all prior writes are visible after this fence
            std::atomic_thread_fence(std::memory_order_acquire);

            get_counter<lifecycle_event::Constructor>().store(0, std::memory_order_relaxed);
            get_counter<lifecycle_event::CopyConstructor>().store(0, std::memory_order_relaxed);
            get_counter<lifecycle_event::MoveConstructor>().store(0, std::memory_order_relaxed);
            get_counter<lifecycle_event::CopyAssignment>().store(0, std::memory_order_relaxed);
            get_counter<lifecycle_event::MoveAssignment>().store(0, std::memory_order_relaxed);
            get_counter<lifecycle_event::Destructor>().store(0, std::memory_order_relaxed);

            // prevent later operations from being reordered before this fence
            std::atomic_thread_fence(std::memory_order_release);
        }

        // Get lifecycle counters
        static QS_CONSTEXPR14 lifecycle_counters get_counters()
        {
            lifecycle_counters res;

            // ensure all prior writes are visible after this fence
            std::atomic_thread_fence(std::memory_order_acquire);

            // read all counters with relaxed memory order
            // order of reading here is not important
            res.constructor =
                get_counter<lifecycle_event::Constructor>().load(std::memory_order_relaxed);
            res.copy_constructor =
                get_counter<lifecycle_event::CopyConstructor>().load(std::memory_order_relaxed);
            res.move_constructor =
                get_counter<lifecycle_event::MoveConstructor>().load(std::memory_order_relaxed);
            res.copy_assignment =
                get_counter<lifecycle_event::CopyAssignment>().load(std::memory_order_relaxed);
            res.move_assignment =
                get_counter<lifecycle_event::MoveAssignment>().load(std::memory_order_relaxed);
            res.destructor =
                get_counter<lifecycle_event::Destructor>().load(std::memory_order_relaxed);

            // prevent later operations from being reordered before this fence
            std::atomic_thread_fence(std::memory_order_release);

            return res;
        }

        // Print lifecycle counters
        static QS_CONSTEXPR14 lifecycle_counters print_counters()
        {
            lifecycle_counters const cnts = get_counters();
            logger_.print_counters(cnts, get_type_name());
            return cnts;
        }

#if defined(__cpp_lib_string_view)
        // Set type name using string_view
        static QS_CONSTEXPR17 void set_type_name(std::string_view const type_name)
        {
            type_name_ = type_name;
        }
#else
        // Set type name using char array
        template<size_t N>
        static QS_CONSTEXPR17 void set_type_name(char const (&type_name)[N])
        {
            type_name_.assign(type_name, N - 1);
        }
        // Set type name using string
        static QS_CONSTEXPR17 void set_type_name(std::string const& type_name)
        {
            type_name_ = type_name;
        }
#endif

        // Get type name
        static QS_CONSTEXPR14 std::string const& get_type_name()
        {
            if(type_name_.empty())
                set_type_name(demangler<T>::get());
            return type_name_;
        }

    protected:
        // Get pointer to derived class
        QS_CONSTEXPR14 Derived*       self() noexcept { return static_cast<Derived*>(this); }
        QS_CONSTEXPR14 Derived const* self() const noexcept
        {
            return static_cast<Derived const*>(this);
        }

    private:
        // Struct to hold atomic counter
        struct alignas(QS_CACHELINE_SIZE) atomic_counter_t
        {
            std::atomic<size_t> value{};
        };

        // Static variables for counters, type name, and logger
        QS_INLINE_VAR static atomic_counter_t                  counters_[6] QS_INLINE_VAR_INIT({});
        QS_INLINE_VAR static std::string type_name_            QS_INLINE_VAR_INIT({});
        QS_INLINE_VAR static lifecycle_logger<T, Uuid> logger_ QS_INLINE_VAR_INIT({});

        // Get reference to specific counter
        template<lifecycle_event Cnt>
        static QS_CONSTEXPR11 std::atomic<size_t>& get_counter()
        {
            return counters_[static_cast<size_t>(Cnt)].value;
        }

        // Log event and increment counter
        template<lifecycle_event Cnt>
        QS_CONSTEXPR17 void log_and_increment() const
        {
            // increment the appropriate counter for the lifecycle event
            get_counter<Cnt>().fetch_add(1, std::memory_order_relaxed);
            // magic of logging occurs here, where we go from Base -> Derived: value_type, Base ->
            // value_type we pass value_type const& reference to the logger which can be used to
            // format the log message logger is customizable
            logger_.template log_event<Cnt>(*static_cast<T const*>(self()), get_type_name());
        }
    };

#if !defined(__cpp_inline_variables)
    template<class Derived, class T, size_t Uuid>
    typename lifecycle_tracker_mt_base<Derived, T, Uuid>::atomic_counter_t
        lifecycle_tracker_mt_base<Derived, T, Uuid>::counters_[6];
    template<class Derived, class T, size_t Uuid>
    std::string lifecycle_tracker_mt_base<Derived, T, Uuid>::type_name_{};
    template<class Derived, class T, size_t Uuid>
    lifecycle_logger<T, Uuid> lifecycle_tracker_mt_base<Derived, T, Uuid>::logger_{};
#endif

} // namespace intl


// Lifecycle tracker class
template<class T, size_t Uuid = 0>
class lifecycle_tracker : public T,
                          public intl::lifecycle_tracker_base<lifecycle_tracker<T, Uuid>, T, Uuid>
{
    using tracker = intl::lifecycle_tracker_base<lifecycle_tracker<T, Uuid>, T, Uuid>;

public:
    using T::T;
    using tracker::get_counters;
    using tracker::get_type_name;
    using tracker::print_counters;
    using tracker::reset_counters;
    using tracker::set_type_name;
};

// Lifecycle tracker class with multi-threading support
template<class T, size_t Uuid = 0>
class lifecycle_tracker_mt
    : public T,
      public intl::lifecycle_tracker_mt_base<lifecycle_tracker_mt<T, Uuid>, T, Uuid>
{
    using tracker = intl::lifecycle_tracker_mt_base<lifecycle_tracker_mt<T, Uuid>, T, Uuid>;

public:
    using T::T;
    using tracker::get_counters;
    using tracker::get_type_name;
    using tracker::print_counters;
    using tracker::reset_counters;
    using tracker::set_type_name;
};

// Specialization for void type
template<size_t Uuid>
class lifecycle_tracker<void, Uuid>
    : public intl::lifecycle_tracker_base<lifecycle_tracker<void, Uuid>,
                                          lifecycle_tracker<void, Uuid>, Uuid>
{
    using tracker = intl::lifecycle_tracker_base<lifecycle_tracker<void, Uuid>,
                                                 lifecycle_tracker<void, Uuid>, Uuid>;

public:
    using tracker::get_counters;
    using tracker::get_type_name;
    using tracker::print_counters;
    using tracker::reset_counters;
    using tracker::set_type_name;
};

template<size_t Uuid>
class lifecycle_tracker_mt<void, Uuid>
    : public intl::lifecycle_tracker_mt_base<lifecycle_tracker<void, Uuid>,
                                             lifecycle_tracker<void, Uuid>, Uuid>
{
    using tracker = intl::lifecycle_tracker_mt_base<lifecycle_tracker_mt<void, Uuid>,
                                                    lifecycle_tracker_mt<void, Uuid>, Uuid>;

public:
    using tracker::get_counters;
    using tracker::get_type_name;
    using tracker::print_counters;
    using tracker::reset_counters;
    using tracker::set_type_name;
};


QS_NAMESPACE_END


#if QS_LIFECYCLE_TRACKER_WITH_FMTLIB

template<class T, size_t Uuid>
struct fmt::formatter<qs::lifecycle_tracker<T, Uuid>> : fmt::formatter<T>
{};

template<class T, size_t Uuid>
struct fmt::formatter<qs::lifecycle_tracker_mt<T, Uuid>> : fmt::formatter<T>
{};

#elif defined(__cpp_lib_print)

template<class T, size_t Uuid>
struct std::formatter<qs::lifecycle_tracker<T, Uuid>> : std::formatter<T>
{};

template<class T, size_t Uuid>
struct std::formatter<qs::lifecycle_tracker_mt<T, Uuid>> : std::formatter<T>
{};

#endif


#undef QS_LIFECYCLE_LOGGER_PRINT
#undef QS_LIFECYCLE_LOGGER_STRING_ARG


#endif // QS_LIFECYCLE_TRACKER_SINGLE_HEADER_H
