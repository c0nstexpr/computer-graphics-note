#pragma once

#include "test.h"

namespace graphics::test
{
    template<typename T>
    using triangle = std::array<glm::vec<2, T, glm::defaultp>, 3>;
}

namespace Catch
{
    template<typename T, std::uniform_random_bit_generator Engine = std::mt19937_64>
    class random_triangle_generator final :
        public Generators::IGenerator<graphics::test::triangle<T>>
    {
        using triangle = graphics::test::triangle<T>;

        random_glm_vec_generator<2, T, Engine> m_gen;
        triangle current_{};

    public:
        random_triangle_generator(const T low, const T high, auto&&... args):
            m_gen(low, high, cpp_forward(args)...)
        {
            next();
        }

        [[nodiscard]] const triangle& get() const override { return current_; }

        bool next() override
        {
            current_[0] = m_gen.get();

            do {
                m_gen.next();
                current_[1] = m_gen.get();
            } while(current_[0] == current_[1]);

            const auto u = current_[1] - current_[0];

            auto cmp = std::strong_ordering::equal;

            do {
                m_gen.next();
                current_[2] = m_gen.get();

                const auto v = current_[2] - current_[1];
                cmp = u.x * v.y <=> u.y * v.x;
            } while(is_eq(cmp));

            if(is_lt(cmp)) std::ranges::swap(current_[0], current_[1]);

            return true;
        }
    };

    template<typename T, std::uniform_random_bit_generator Engine = std::mt19937_64>
    Generators::GeneratorWrapper<graphics::test::triangle<T>>
        random_triangle(const T low, decltype(low) high, auto&&... args)
    { // NOLINTNEXTLINE(*-owning-memory)
        return new random_triangle_generator<T, Engine>{low, high, cpp_forward(args)...};
    }
}