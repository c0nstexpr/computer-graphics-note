#pragma once

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/matchers/catch_matchers_range_equals.hpp>
#include <format>
#include <glm/glm.hpp>

namespace glm
{
    template<typename CharT, typename T, qualifier Qualifier>
    std::basic_ostream<CharT>&
        operator<<(std::basic_ostream<CharT>& os, const glm::vec<2, T, Qualifier>& v)
    {
        return os << std::format("{{{}, {}}}", v.x, v.y);
    }
}