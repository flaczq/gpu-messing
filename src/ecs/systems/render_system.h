#pragma once

class Registry;

class RenderSystem {
public:
	RenderSystem();

	void update(Registry& registry, float alpha);
};