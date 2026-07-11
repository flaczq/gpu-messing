#pragma once

#include "../components/physics_component.h"
#include "../configs/math_config.hpp"
#include "../graphics/renderer.h"
#include <unordered_map>
#include <vector>

class TransformComponent;

enum class PhysicsCommandType {
	ADD,
	REMOVE
};

struct PhysicsBody {
	const TransformComponent* transform;
	AABB AABB;
};
struct PhysicsCommand {
	std::string name;
	PhysicsCommandType commandType;
	const PhysicsBody* physicsBody;
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

	std::vector<RendererImmediateCommand> getAABBCommand();

private:
	// hidden constructor
	PhysicsWorld();

	std::vector<PhysicsCommand> m_physicsQueue;
	std::unordered_map<std::string, PhysicsBody> m_physicsBodies;

	unsigned int m_VAOAABB{}, m_VBOAABB{};
};