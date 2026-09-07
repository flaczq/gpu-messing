#pragma once

class Component {
public:
	virtual ~Component();
	Component(const Component&) = delete;
	Component& operator=(const Component&) = delete;

	virtual void onFixedUpdate(float fixedt) {}
	virtual void onUpdate(float alpha) const {}

protected:
	Component() = default;

private:
	//private
};