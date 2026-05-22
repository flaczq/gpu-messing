#pragma once

#include "../configs/math_config.hpp"
#include <string>
#include <vector>

class TransformComponent;

enum class PhysicsCommandType {
	ADD,
	REMOVE
};

struct PhysicsCommand {
	const std::string& name;
	PhysicsCommandType commandType;
	TransformComponent* transform;
};
struct PhysicsBody {
	const std::string& name;
	glm::vec3 position;
	glm::vec3 size;
	glm::vec3 color;
};

class PhysicsWorld {
public:
	// Meyer's Singleton
	static PhysicsWorld& getInstance();
	PhysicsWorld(const PhysicsWorld&) = delete;
	void operator=(const PhysicsWorld&) = delete;
	~PhysicsWorld();

	bool init();
	void registerInQueue(const PhysicsCommand& command);
	void flush();
	void step(float fixedt) const;

private:
	// hidden constructor
	PhysicsWorld();

	std::vector<PhysicsCommand> m_physicsQueue;
	std::vector<PhysicsBody> m_physicsBodies;
};