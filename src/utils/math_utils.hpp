#pragma once

#include "../configs/math_config.hpp"
#include <string>

namespace Utils {
	constexpr std::string getVec3Values(glm::vec3 value) {
		return "(" + std::to_string(value.x) + ", " +
					 std::to_string(value.y) + ", " +
					 std::to_string(value.z) + ")";
	}
}