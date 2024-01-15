#pragma once

#include "test.h"

namespace graphics::test
{
    template<typename T>
    struct triangle
    {
        using vec = glm::vec<2, T, glm::defaultp>;

        vec p0{};
        vec p1{};
        vec p2{};
    };
}

namespace Catch
{
    template<typename T>
    class random_triangle_generator final :
        public Generators::IGenerator<graphics::test::triangle<T>>
    {
        using triangle = graphics::test::triangle<T>;

        random_glm_vec_generator<2, T> m_gen;
        triangle current_{};

    public:
        random_triangle_generator(const T low, const T high, const seed_t seed = default_seed):
            m_gen(low, high, seed)
        {
            next();
        }

        [[nodiscard]] const triangle& get() const override { return current_; }

        bool next() override
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
    };

    template<typename T>
    Generators::GeneratorWrapper<graphics::test::triangle<T>>
        random_triangle(const T low, decltype(low) high)
    {
        return new random_triangle_generator{low, high}; // NOLINT(*-owning-memory)
    }
}