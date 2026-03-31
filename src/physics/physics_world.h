#pragma once

class PhysicsWorld {
public:
	PhysicsWorld();

	bool init();
	void fixedUpdate(float fixedt) const;
	void saveState();

	void togglePhysics();

private:
	bool m_active{};
};