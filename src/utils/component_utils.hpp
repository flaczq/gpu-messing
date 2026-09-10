#pragma once

#include "../configs/math_config.hpp"
#include "../ecs/components/transform_component.hpp"
#include "stats_constants.hpp"

namespace Utils {
	namespace Component {
		constexpr glm::vec3 getInterpolatedPosition(const TransformComponent& transform, float alpha) {
			glm::vec3 interPosition = glm::mix(transform.prevPosition, transform.position, alpha);
			return interPosition;
		}

		constexpr glm::mat4 getInterpolatedModelMatrix(const TransformComponent& transform, float alpha) {
			glm::vec3 interPosition = getInterpolatedPosition(transform, alpha);
			glm::quat interRotation = glm::slerp(transform.prevRotation, transform.rotation, alpha);
			glm::vec3 interScale = glm::mix(transform.prevScale, transform.scale, alpha);
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, interPosition);
			model *= glm::mat4_cast(interRotation);
			model = glm::scale(model, interScale);
			return model;
		}

		constexpr glm::mat4 getNormalMatrix(const glm::mat4& model) {
			glm::mat4 normal = glm::transpose(glm::inverse(glm::mat3(model)));
			return normal;
		}

		constexpr glm::vec3 getFront(const TransformComponent& transform) {
			glm::vec3 front = glm::vec3(0.0f, 0.0f, 0.0f);
			front.x = cos(glm::radians(transform.yaw)) * cos(glm::radians(transform.pitch));
			front.y = sin(glm::radians(transform.pitch));
			front.z = sin(glm::radians(transform.yaw)) * cos(glm::radians(transform.pitch));
			return glm::normalize(front);
		}

		constexpr glm::vec3 getFlatFront(const TransformComponent& transform) {
			glm::vec3 flatFront = glm::vec3(0.0f, 0.0f, 0.0f);
			flatFront.x = cos(glm::radians(transform.yaw));
			flatFront.y = 0.0f;
			flatFront.z = sin(glm::radians(transform.yaw));
			return glm::normalize(flatFront);
		}

		constexpr glm::vec3 getRight(const TransformComponent& transform) {
			return glm::normalize(glm::cross(getFront(transform), Constants::Stats::World::WORLD_UP));
		}

		constexpr glm::vec3 getUp(const TransformComponent& transform) {
			return glm::normalize(glm::cross(getRight(transform), getFront(transform)));
		}
	}
}