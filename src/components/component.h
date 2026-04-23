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
	virtual void onFixedUpdate(float fixedt) {}
	virtual void onUpdate(float alpha) const {}
	virtual void onEnd() {}

	GameEntity* getOwner() const { return m_owner; }

protected:
	Component() = default;

private:
	GameEntity* m_owner = nullptr;
};