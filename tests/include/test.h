#pragma once

#include <format>
#include <ranges>
#include <random>

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_get_random_seed.hpp>
#include <catch2/generators/catch_generators_random.hpp>
#include <catch2/generators/catch_generators_adapters.hpp>
#include <catch2/matchers/catch_matchers_range_equals.hpp>

#include <glm/glm.hpp>

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

namespace glm
{
    template<typename CharT, typename T, glm::length_t L, qualifier Qualifier>
    std::basic_ostream<CharT>&
        operator<<(std::basic_ostream<CharT>& os, const glm::vec<L, T, Qualifier>& v)
    {
        return os << std::format("{}", v);
    }
}

std::mt19937_64& get_mt_engine();

namespace Catch
{
    template<glm::length_t L, typename T, glm::qualifier Q = glm::defaultp>
    class random_glm_vec_generator final : public Generators::IGenerator<glm::vec<L, T, Q>>
    {
        using dist_t = std::conditional_t<
            std::floating_point<T>,
            std::uniform_real_distribution<T>,
            std::uniform_int_distribution<T>>;

        using typename Generators::IGenerator<glm::vec<L, T, Q>>::type;

        dist_t m_dist;
        type current_{};

    public:
        random_glm_vec_generator(const T low, const T high): m_dist(low, high) { next(); }

        const type& get() const override { return current_; }

        bool next() override
        {
            for(glm::length_t i = 0; i < L; ++i) current_[i] = m_dist(get_mt_engine());
            return true;
        }
    };

    template<glm::length_t L, typename T, glm::qualifier Q = glm::defaultp>
    Generators::GeneratorWrapper<glm::vec<L, T, Q>> random_glm_vec(const T low, decltype(low) high)
    {
        return new random_glm_vec_generator<L, T, Q>{low, high};
    }
}