#pragma once

#include <format>
#include <ranges>
#include <random>

#include <catch2/generators/catch_generators_all.hpp>
#include <catch2/catch_get_random_seed.hpp>
#include <catch2/matchers/catch_matchers_all.hpp>
#include <catch2/reporters/catch_reporters_all.hpp>
#include <catch2/catch_test_macros.hpp>

#include <glm/glm.hpp>

#include <stdsharp/concepts/concepts.h>

namespace std
{
    template<typename CharT, typename T, glm::length_t L, glm::qualifier Qualifier>
    struct formatter<glm::vec<L, T, Qualifier>, CharT> : std::range_formatter<T, CharT>
    {
        using range_formatter = std::range_formatter<T, CharT>;

        static constexpr std::ranges::iota_view<glm::length_t, glm::length_t> indices{0, L};

        template<typename FormatContext>
        constexpr auto format(const glm::vec<L, T, Qualifier>& v, FormatContext& ctx) const
        {
            return range_formatter::format(
                indices | std::views::transform([&v](const auto i) { return v[i]; }),
                ctx
            );
        }
    };
}

template<std::uniform_random_bit_generator T = std::mt19937_64>
struct random_engine_wrapper : std::reference_wrapper<T>
{
    using std::reference_wrapper<T>::reference_wrapper;

    using result_type = typename T::result_type;

    static constexpr auto min() { return T::min(); }
    static constexpr auto max() { return T::max(); }
};

namespace glm
{
    template<typename CharT, typename T, glm::length_t L, qualifier Qualifier>
    std::basic_ostream<CharT>&
        operator<<(std::basic_ostream<CharT>& os, const glm::vec<L, T, Qualifier>& v)
    {
        return os << std::format("{}", v);
    }
}

namespace Catch
{
    template<
        glm::length_t L,
        typename T,
        std::uniform_random_bit_generator Engine = std::mt19937_64>
    class random_glm_vec_generator final : public Generators::IGenerator<glm::vec<L, T>>
    {
    public:
        using typename Generators::IGenerator<glm::vec<L, T>>::type;

    private:
        using dist_t = std::conditional_t<
            std::floating_point<T>,
            std::uniform_real_distribution<T>,
            std::uniform_int_distribution<T>>;

        dist_t m_dist;
        Engine m_engine;
        type current_{};

    public:
        random_glm_vec_generator(const T low, const T high, auto&&... args):
            m_dist(low, high), m_engine(cpp_forward(args)...)
        {
            next();
        }

        const type& get() const override { return current_; }

        bool next() override
        {
            for(glm::length_t i = 0; i < L; ++i) current_[i] = m_dist(m_engine);
            return true;
        }

        auto& engine() { return m_engine; }
    };

    template<
        glm::length_t L,
        typename T,
        std::uniform_random_bit_generator Engine = std::mt19937_64>
    Generators::GeneratorWrapper<glm::vec<L, T>>
        random_glm_vec(const T low, decltype(low) high, auto&&... args)
    { // NOLINTNEXTLINE(*-owning-memory)
        return new random_glm_vec_generator<L, T, Engine>{low, high, cpp_forward(args)...};
    }
}