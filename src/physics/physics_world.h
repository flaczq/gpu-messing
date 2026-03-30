#pragma once

class PhysicsWorld {
public:
	PhysicsWorld();

	bool init();
	void fixedUpdate(float fixedt) const;
	void savePreviousState();

	void togglePhysics();

private:
	bool m_active{};
};