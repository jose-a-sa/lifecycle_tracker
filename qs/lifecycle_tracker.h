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


#ifndef QS_LIFECYCLE_TRACKER_H
#define QS_LIFECYCLE_TRACKER_H

#include <qs/config.h>
#include <qs/demangler.h>

// Include necessary headers
#include <array>
#include <atomic>
#include <cstddef>
#include <string>
#include <tuple>
#include <type_traits>

#if defined(__cpp_lib_string_view)
#include <string_view>
#endif

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
            // memory_order_relaxed is used as the order of increments in this step is not really
            // important
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


#endif // QS_LIFECYCLE_TRACKER_H
