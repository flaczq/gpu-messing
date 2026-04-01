#pragma once

class PhysicsWorld {
public:
	PhysicsWorld();

	void init();
	void fixedUpdate(float fixedt) const;
	void saveState();

	bool isActive() const { return m_active; }
	void setActive(bool active) { m_active = active; }

private:
	bool m_active{};
};