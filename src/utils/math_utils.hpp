#pragma once

#include "../configs/math_config.hpp"
#include "stats_constants.hpp"
#include <string>

namespace Utils {
	namespace Math {
		constexpr std::string getVec3Values(const glm::vec3& value) {
			return "(" + std::to_string(value.x) + ", "
					   + std::to_string(value.y) + ", "
					   + std::to_string(value.z) + ")";
		}

		inline glm::vec3 calculateInterpolatedPosition(const glm::vec3& position, const glm::vec3& prevPosition, float alpha) {
			return glm::mix(prevPosition, position, alpha);
		}
		inline glm::mat4 calculateInterpolatedModelMatrix(const glm::vec3& position,
														  const glm::vec3& prevPosition,
														  const glm::quat& rotation,
														  const glm::quat& prevRotation,
														  const glm::vec3& scale,
														  const glm::vec3& prevScale,
														  float alpha) {
			glm::vec3 interPosition = calculateInterpolatedPosition(position, prevPosition, alpha);
			glm::quat interRotation = glm::slerp(prevRotation, rotation, alpha);
			glm::vec3 interScale = glm::mix(prevScale, scale, alpha);
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, interPosition);
			model *= glm::mat4_cast(interRotation);
			model = glm::scale(model, interScale);
			return model;
		}
		inline glm::mat4 calculateNormalMatrix(const glm::mat4& model) {
			return glm::transpose(glm::inverse(glm::mat3(model)));
		}

		inline glm::vec3 calculateFront(float yaw, float pitch) {
			glm::vec3 front = glm::vec3(0.0f, 0.0f, 0.0f);
			front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
			front.y = sin(glm::radians(pitch));
			front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
			return glm::normalize(front);
		}
		inline glm::vec3 calculateFlatFront(float yaw) {
			glm::vec3 flatFront = glm::vec3(0.0f, 0.0f, 0.0f);
			flatFront.x = cos(glm::radians(yaw));
			flatFront.y = 0.0f;
			flatFront.z = sin(glm::radians(yaw));
			return glm::normalize(flatFront);
		}
		inline glm::vec3 calculateRight(float yaw, float pitch) {
			return glm::normalize(glm::cross(calculateFront(yaw, pitch), Constants::Stats::World::WORLD_UP));
		}
		inline glm::vec3 calculateUp(float yaw, float pitch) {
			return glm::normalize(glm::cross(calculateRight(yaw, pitch), calculateFront(yaw, pitch)));
		}
		inline glm::mat4 calculateView(const glm::vec3& position,
									   const glm::vec3& prevPosition,
									   float yaw,
									   float pitch,
									   float alpha,
									   float yOffset = 0.0f) {
			glm::vec3 interPosition = calculateInterpolatedPosition(position, prevPosition, alpha);
			glm::vec3 front = calculateFront(yaw, pitch);
			glm::vec3 up = calculateUp(yaw, pitch);
			interPosition.y += yOffset;
			// followed position, where you looking at, up vector
			return glm::lookAt(interPosition, interPosition + front, up);
		}
		inline glm::mat4 calculateProjection(float fov, float aspect, float nearPlane, float farPlane) {
			return glm::perspective(glm::radians(fov), aspect, nearPlane, farPlane);
		}

		constexpr glm::vec3 calculateSize(const glm::vec3& min, const glm::vec3& max) {
			return max - min;
		}
		constexpr glm::vec3 calculateCenter(const glm::vec3& min, const glm::vec3& max) {
			return (min + max) * 0.5f;
		}
	}
}