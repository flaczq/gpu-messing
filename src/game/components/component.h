#pragma once

class GameObject;

class Component {
public:
	virtual ~Component() = default;

	virtual void fixedUpdate(float fixedt) {}
	virtual void renderFrame(float alpha) {}

	unsigned int getTypeID() const { return m_typeID; }
private:
	GameObject* m_owner = nullptr;
	unsigned int m_typeID{};
};