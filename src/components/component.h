#pragma once

class GameEntity;

class Component {
	friend class GameEntity;

public:
	virtual ~Component();

	GameEntity* getOwner() const { return m_owner; }

protected:
	Component() = default;

private:
	GameEntity* m_owner = nullptr;
};