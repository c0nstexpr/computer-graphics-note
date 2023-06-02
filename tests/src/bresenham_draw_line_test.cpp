#include <catch2/matchers/catch_matchers_range_equals.hpp>

#include "test.h"
#include "graphics/bresenham_draw_line.h"

using namespace std;
using namespace stdsharp;
using namespace glm;
using namespace graphics;

SCENARIO("draw test", "[bresenham_draw_line]")
{
    struct arg
    {
        ivec2 p0;
        decltype(p0) p1;
        decltype(p0)::value_type dx;

        vector<decltype(p0)> out;
    };

    using const_iterator = decltype(arg{}.out)::const_iterator;

    struct catch_require_iterator : const_iterator
    {
        using const_iterator::const_iterator;

        catch_require_iterator(const const_iterator iter): const_iterator(iter) {}

        catch_require_iterator& operator*() { return *this; }

        catch_require_iterator& operator=(const ivec2 value)
        {
            REQUIRE(value == *static_cast<const_iterator&>(*this));
            return *this;
        }

        catch_require_iterator& operator++()
        {
            static_cast<const_iterator&>(*this)++;
            return *this;
        }

        catch_require_iterator operator++(int)
        {
            const auto result = *this;
            ++*this;
            return result;
        }
    };

    const auto args = GENERATE(
        arg{
            {0, 0},
            {3, 3},
            1,
            {{0, 0}, {1, 1}, {2, 2}, {3, 3}} //
        },
        arg{
            {0, 0},
            {5, 2},
            1,
            {{0, 0}, {1, 0}, {2, 1}, {3, 1}, {4, 2}, {5, 2}} //
        },
        arg{
            {0, 0},
            {2, 5},
            1,
            {{0, 0}, {1, 1}, {1, 2}, {1, 3}, {2, 4}, {2, 5}} //
        }
    );

    bresenham_draw_line(args.p0, args.p1, catch_require_iterator{args.out.cbegin()}, args.dx);
}