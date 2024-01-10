#include "triangle.h"

namespace Catch
{
    random_triangle_generator::random_triangle_generator(const int low, const int high):
        m_gen(low, high)
    {
        next();
    }

    const Generators::IGenerator<graphics::test::triangle>::type&
        random_triangle_generator::get() const
    {
        return current_;
    }

    bool random_triangle_generator::next()
    {
        current_.p0 = m_gen.get();

        do {
            m_gen.next();
            current_.p1 = m_gen.get();
        } while(current_.p0 == current_.p1);

        const auto line_a = current_.p1.y - current_.p0.y;
        const auto line_b = current_.p0.x - current_.p1.x;
        const auto line_c = line_a * current_.p0.x + line_b * current_.p0.y;

        do {
            m_gen.next();
            current_.p2 = m_gen.get();
        } while((line_a * current_.p2.x + line_b * current_.p2.y) == line_c);

        return true;
    }

    Generators::GeneratorWrapper<graphics::test::triangle>
        random_triangle(const int low, const int high)
    {
        return new random_triangle_generator{low, high}; // NOLINT(*-owning-memory)
    }
}