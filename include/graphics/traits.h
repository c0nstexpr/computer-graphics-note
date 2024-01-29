#pragma once

#include <iterator>

#include <stdsharp/cassert/cassert.h>
#include <stdsharp/default_operator.h>

#include "namespace_alias.h"

namespace graphics::details
{
    template<typename Vec>
    class glm_vec_iterator :
        public std::random_access_iterator_tag,
        public star::default_arithmetic_operation<glm_vec_iterator<Vec>>
    {
    public:
        using difference_type = std::make_signed_t<glm::length_t>;
        using value_type = Vec::value_type;

    private:
        Vec* vec_ = nullptr;
        glm::length_t index_ = Vec::length();

    public:
        glm_vec_iterator() = default;

        constexpr glm_vec_iterator(Vec& vec, const glm::length_t index = 0) noexcept:
            vec_(&vec), index_(index)
        {
        }

        constexpr auto& operator*() const noexcept { return (*vec_)[index_]; }

        constexpr auto& operator[](const difference_type diff) const noexcept
        {
            return (*vec_)[index_ + diff];
        }

        constexpr auto& operator+=(const difference_type diff) noexcept
        {
            index_ += diff;
            return *this;
        }

        constexpr auto& operator-=(const difference_type diff) noexcept
        {
            index_ -= diff;
            return *this;
        }

        constexpr auto operator-(const glm_vec_iterator& rhs) const noexcept
        {
            return static_cast<difference_type>(index_) - rhs.index_;
        }

        friend auto operator+(const difference_type diff, glm_vec_iterator rhs) noexcept
        {
            return cpp_move(rhs) += diff;
        }

        friend auto operator-(const difference_type diff, glm_vec_iterator rhs) noexcept
        {
            return cpp_move(rhs) -= diff;
        }

        [[nodiscard]] constexpr bool operator==(const glm_vec_iterator& other) const noexcept
        {
            return (vec_ == other.vec_) && (index_ == other.index_);
        }

        [[nodiscard]] constexpr auto operator<=>(const glm_vec_iterator& other) const noexcept
        {
            Expects(vec_ == other.vec_);
            return index_ <=> other.index_;
        }
    };
}

namespace graphics
{
    template<typename>
    struct glm_vec_traits;

    template<typename T, glm::qualifier Q, glm::length_t L>
    struct glm_vec_traits<glm::vec<L, T, Q>>
    {
        using vec = glm::vec<L, T, Q>;
        using value_type = T;
        static constexpr auto qualifier = Q;
        static constexpr auto length = L;
    };

    template<glm::length_t L, typename T, glm::qualifier Q>
    using glm_vec_iterator = details::glm_vec_iterator<glm::vec<L, T, Q>>;

    template<glm::length_t L, typename T, glm::qualifier Q>
    using glm_vec_const_iterator = details::glm_vec_iterator<const glm::vec<L, T, Q>>;
}

namespace glm
{
    template<glm::length_t L, typename T, glm::qualifier Q>
    constexpr ::graphics::glm_vec_iterator<L, T, Q> begin(glm::vec<L, T, Q>& vec) noexcept
    {
        return {vec};
    }

    template<glm::length_t L, typename T, glm::qualifier Q>
    constexpr ::graphics::glm_vec_iterator<L, T, Q> end(glm::vec<L, T, Q>& vec) noexcept
    {
        return {vec, L};
    }

    template<glm::length_t L, typename T, glm::qualifier Q>
    constexpr ::graphics::glm_vec_const_iterator<L, T, Q> begin( //
        const glm::vec<L, T, Q>& vec
    ) noexcept
    {
        return {vec};
    }

    template<glm::length_t L, typename T, glm::qualifier Q>
    constexpr ::graphics::glm_vec_const_iterator<L, T, Q> end(const glm::vec<L, T, Q>& vec) noexcept
    {
        return {vec, L};
    }

    template<glm::length_t L, typename T, glm::qualifier Q>
    constexpr ::graphics::glm_vec_const_iterator<L, T, Q>
        size(const glm::vec<L, T, Q>& /*vec*/) noexcept
    {
        return L;
    }
}
