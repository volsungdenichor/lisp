#pragma once

namespace lisp
{
struct null_t
{
    null_t() = default;
    null_t(const null_t&) = default;
    null_t(null_t&&) = default;
};

static constexpr inline auto null = null_t{};

}  // namespace lisp