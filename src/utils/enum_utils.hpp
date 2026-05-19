#pragma once

// for all GL const
#define MAGIC_ENUM_RANGE_MIN 0
#define MAGIC_ENUM_RANGE_MAX 10000

#include <magic_enum.hpp>
#include <string_view>

namespace Utils {
    template <typename T>
    constexpr T getEnumNext(T current) {
        auto values = magic_enum::enum_values<T>();
        auto index = magic_enum::enum_index(current).value_or(0);
        auto nextIndex = (index + 1) % values.size();
        return values[nextIndex];
    }

    template <typename T>
    constexpr std::string_view getEnumName(T value) {
        return magic_enum::enum_name(value);
    }
}