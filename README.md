# lifecycle_tracker
Wrapper template class to count constructor, destructor, assignments in C++11 and above


## Basic usage

```cpp
struct MyInt
{
    int v{};
    MyInt(int v) : v{v} {};
    constexpr bool operator==(MyInt const& rhs) const { return v == rhs.v; }
};

template<size_t Uuid>
struct qs::lifecycle_logger<std::MyInt, Uuid> : qs::lifecycle_default_logger<std::MyInt, Uuid>
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

{
    std::vector<qs::lifecycle_tracker<MyInt>> vec;
    vec.reserve(100);

    vec.emplace_back(42);
    // MyInt(...) -> 42
    vec.insert(vec.cend(), {11, 17, 20} /*implicit conversion*/);
    // MyInt(...) -> 11
    // MyInt(...) -> 17
    // MyInt(...) -> 20
    // MyInt(MyInt const&) -> 11
    // MyInt(MyInt const&) -> 17
    // MyInt(MyInt const&) -> 20
    // ~MyInt() -> 20
    // ~MyInt() -> 17
    // ~MyInt() -> 11
    qs::lifecycle_tracker<MyInt>::print_counters();
    // Lifecycle tracker [type: MyInt, uuid: 0]
    //  * constructor (ctor/copy/move) :     7 (4/3/0)
    //  * assign (copy/move)           :     0 (0/0)
    //  * destructor (alive)           :     3 (4)
    vec.assign({23});
    // MyInt(...) -> 23
    // =(MyInt const&) -> 23
    // ~MyInt() -> 20
    // ~MyInt() -> 17
    // ~MyInt() -> 11
    // ~MyInt() -> 23
}
// ~MyInt() -> 23

qs::lifecycle_tracker<MyInt>::print_counters();
// Lifecycle tracker [type: MyInt, uuid: 0]
//  * constructor (ctor/copy/move) :     8 (5/3/0)
//  * assign (copy/move)           :     1 (1/0)
//  * destructor (alive)           :     8 (0)
```
