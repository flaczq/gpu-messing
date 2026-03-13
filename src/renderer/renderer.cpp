#include "renderer.h"

Renderer::Renderer() {
}

RenderMode Renderer::getRenderMode() const {
	return renderMode;
}

void Renderer::setRenderMode(RenderMode n_renderMode) {
	renderMode = n_renderMode;
}