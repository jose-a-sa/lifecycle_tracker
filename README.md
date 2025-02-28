# lifecycle_tracker

The `lifecycle_tracker` provides a template class that wraps around other classes to count the number of times constructors, destructors, and assignment operators are called. This can be useful for debugging and understanding the lifecycle of objects in C++.

## Features

- Supports C++11 and above
- Counts constructors, copy constructors, move constructors, destructors, copy and move assignment operators
- Customizable logging and formatting
- Thread-safe counters available

## Usage

The main usage of `qs::lifecycle_tracker<T, Uuid>` is to keep track of the object creation and destruction using static counters. An object of `qs::lifecycle_tracker<T, Uuid>` can be used instead of a type `T` (where `T` must be a class). We can specify a `Uuid` to distinguish different sets of counters.

```cpp
qs::lifecycle_tracker<std::string, 42> my_tracked_str = "abracadabra";
```

The tracker contains a few static methods:
- `qs::lifecycle_tracker<T, Uuid>::get_counters()`
- `qs::lifecycle_tracker<T, Uuid>::reset_counters()`
- `qs::lifecycle_tracker<T, Uuid>::print_counters()`

Additionally, it includes methods to specify and get the type name used during logging:
- `qs::lifecycle_tracker<T, Uuid>::get_type_name()`
- `qs::lifecycle_tracker<T, Uuid>::set_type_name(...)`

## Detailed Example

```cpp
struct MyInt
{
    int v;
    MyInt(int v) : v{v} {};
};

{
    std::vector<qs::lifecycle_tracker<MyInt>> vec;
    vec.reserve(100);

    vec.emplace_back(42);
    // MyInt(...)
    vec.insert(vec.cend(), {11, 17, 20} /*implicit conversion*/);
    // MyInt(...)
    // MyInt(...)
    // MyInt(...)
    // MyInt(MyInt const&)
    // MyInt(MyInt const&)
    // MyInt(MyInt const&)
    // ~MyInt()
    // ~MyInt()
    // ~MyInt()
    qs::lifecycle_tracker<MyInt>::print_counters();
    // Lifecycle tracker [type: MyInt, uuid: 0]
    //  * constructor (ctor/copy/move) :     7 (4/3/0)
    //  * assign (copy/move)           :     0 (0/0)
    //  * destructor (alive)           :     3 (4)
    vec.assign({23});
    // MyInt(...)
    // =(MyInt const&)
    // ~MyInt()
    // ~MyInt()
    // ~MyInt()
    // ~MyInt()
}
// ~MyInt()

qs::lifecycle_tracker<MyInt>::print_counters();
// Lifecycle tracker [type: MyInt, uuid: 0]
//  * constructor (ctor/copy/move) :     8 (5/3/0)
//  * assign (copy/move)           :     1 (1/0)
//  * destructor (alive)           :     8 (0)
```

## Custom Formatting and Logging

The logger follows the following concept:

```cpp
template<class T, size_t Uuid = 0>
struct lifecycle_logger
{
    // Logs a specific lifecycle event for the given type instance
    template<lifecycle_event Cnt>
    void log_event(T const& self, std::string const& type_name) const;

    // Prints the current lifecycle counters for the given type
    void print_counters(lifecycle_counters const& cnts, std::string const& type_name) const;
};
```

You can inherit the default behavior of the `qs::lifecycle_default_logger` and customize it:

```cpp
template<size_t Uuid>
struct qs::lifecycle_logger<MyInt, Uuid> : qs::lifecycle_default_logger<MyInt, Uuid>
{
    template<qs::lifecycle_event Cnt>
    void log_event(MyInt const& self, std::string const& type_name) const
    {
        auto const n = static_cast<int>(type_name.size());
        std::printf(event_fmt_map[static_cast<size_t>(Cnt)], n, type_name.c_str(), n, type_name.c_str());
        std::printf(" -> {v: %d}\n", self.v);
    }
private:
    inline static constexpr std::array<char const*, 6> event_fmt_map{
        "%.*s(...)", "%.*s(%.*s const&)", "%.*s(%.*s&&)", "=(%.*s const&)", "=(%.*s&&)", "~%.*s()"};
};
```

## Custom Type Names

The default behavior uses `typeid` and demangling to obtain the nicest possible representation of the type, which is passed to the logging function. Even after demangling the type name, the type can have long names due to template parameters, aliasing, and inline namespaces. We can set a nicer name by using `qs::lifecycle_tracker<T, Uuid>::set_type_name(...)`.

For example, on clang:
```cpp
auto&& t = qs::lifecycle_tracker<std::string>::get_type_name();
// std::__1::basic_string<char, std::char_traits<char>, std::allocator<char>>

qs::lifecycle_tracker<std::string>::set_type_name("std::string");
```