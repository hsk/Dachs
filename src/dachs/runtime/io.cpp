#include <string>
#include <cstdio>
#include <cstdint>

extern "C" {
    void __dachs_println_float__(double const d)
    {
        std::printf("%lg\n", d);
    }

    void __dachs_println_int__(std::int64_t const i)
    {
        std::printf("%lld\n", i);
    }

    void __dachs_println_uint__(std::uint64_t const u)
    {
        std::printf("%llu\n", u);
    }

    void __dachs_println_char__(char const c)
    {
        std::printf("%c\n", c);
    }

    void __dachs_println_string__(char const* const s)
    {
        std::printf("%s\n", s);
    }

    void __dachs_println_symbol__(char const* const s)
    {
        std::printf("%s\n", s);
    }

    void __dachs_println_bool__(bool const b)
    {
        std::printf("%s\n", b ? "true" : "false");
    }

    void __dachs_print_float__(double const d)
    {
        std::printf("%lg", d);
    }

    void __dachs_print_int__(std::int64_t const i)
    {
        std::printf("%lld", i);
    }

    void __dachs_print_uint__(std::uint64_t const u)
    {
        std::printf("%llu", u);
    }

    void __dachs_print_char__(char const c)
    {
        std::printf("%c", c);
    }

    void __dachs_print_string__(char const* const s)
    {
        std::printf("%s", s);
    }

    void __dachs_print_symbol__(char const* const s)
    {
        std::printf("%s", s);
    }

    void __dachs_print_bool__(bool const b)
    {
        std::printf("%s", b ? "true" : "false");
    }
}
