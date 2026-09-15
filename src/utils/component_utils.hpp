#pragma once

#include "../configs/math_config.hpp"
#include "../ecs/components/camera_component.hpp"
#include "../ecs/components/physics_component.hpp"
#include "../ecs/components/transform_component.hpp"
#include "stats_constants.hpp"

namespace Utils {
	namespace Component {
		// TRANSFORM COMPONENT
		inline glm::vec3 calculateInterpolatedPosition(const TransformComponent& transform, float alpha) {
			return glm::mix(transform.prevPosition, transform.position, alpha);
		}
		inline glm::mat4 calculateInterpolatedModelMatrix(const TransformComponent& transform, float alpha) {
			glm::vec3 interPosition = calculateInterpolatedPosition(transform, alpha);
			glm::quat interRotation = glm::slerp(transform.prevRotation, transform.rotation, alpha);
			glm::vec3 interScale = glm::mix(transform.prevScale, transform.scale, alpha);
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, interPosition);
			model *= glm::mat4_cast(interRotation);
			model = glm::scale(model, interScale);
			return model;
		}
		inline glm::mat4 calculateNormalMatrix(const glm::mat4& model) {
			return glm::transpose(glm::inverse(glm::mat3(model)));
		}
		inline glm::vec3 calculateFront(const TransformComponent& transform) {
			glm::vec3 front = glm::vec3(0.0f, 0.0f, 0.0f);
			front.x = cos(glm::radians(transform.yaw)) * cos(glm::radians(transform.pitch));
			front.y = sin(glm::radians(transform.pitch));
			front.z = sin(glm::radians(transform.yaw)) * cos(glm::radians(transform.pitch));
			return glm::normalize(front);
		}
		inline glm::vec3 calculateFlatFront(const TransformComponent& transform) {
			glm::vec3 flatFront = glm::vec3(0.0f, 0.0f, 0.0f);
			flatFront.x = cos(glm::radians(transform.yaw));
			flatFront.y = 0.0f;
			flatFront.z = sin(glm::radians(transform.yaw));
			return glm::normalize(flatFront);
		}
		inline glm::vec3 calculateRight(const TransformComponent& transform) {
			return glm::normalize(glm::cross(calculateFront(transform), Constants::Stats::World::WORLD_UP));
		}
		inline glm::vec3 calculateUp(const TransformComponent& transform) {
			return glm::normalize(glm::cross(calculateRight(transform), calculateFront(transform)));
		}

		// PHYSICS COMPONENT
		constexpr glm::vec3 calculateAABBSize(AABB aabb) {
			return aabb.worldMax - aabb.worldMin;
		}
		constexpr glm::vec3 calculateAABBCenter(AABB aabb) {
			return (aabb.worldMin + aabb.worldMax) * 0.5f;
		}

		// CAMERA COMPONENT
		inline glm::mat4 calculateView(const TransformComponent& transform, float alpha, float yOffset = 0.0f) {
			glm::vec3 interPosition = calculateInterpolatedPosition(transform, alpha);
			glm::vec3 front = calculateFront(transform);
			glm::vec3 up = calculateUp(transform);
			interPosition.y += yOffset;
			// followed position, where you looking at, up vector
			return glm::lookAt(interPosition, interPosition + front, up);
		}
		inline glm::mat4 calculateProjection(float fov, float aspect, float nearPlane, float farPlane) {
			return glm::perspective(glm::radians(fov), aspect, nearPlane, farPlane);
		}
	}
}