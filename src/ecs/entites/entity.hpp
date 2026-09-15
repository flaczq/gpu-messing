#pragma once

#include <cstdint>

// 99.1% pure ecs
using Entity = std::uint32_t;

// invalid entity flag
inline constexpr Entity NULL_ENTITY = 0;