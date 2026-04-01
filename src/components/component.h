#pragma once

class GameEntity;

class Component {
	// compPtr->m_owner = this;
	friend class GameEntity;

public:
	virtual ~Component();
	Component(const Component&) = delete;
	Component& operator=(const Component&) = delete;

	virtual void onInit() {}
	virtual void onUpdate(float dt) {}
	virtual void onLateUpdate(float dt) {}

	bool isActive() const { return m_active; }
	void setActive(bool active) { m_active = active; }
	GameEntity* getOwner() const { return m_owner; }

protected:
	Component() = default;

private:
	bool m_active = true;
	GameEntity* m_owner = nullptr;
};