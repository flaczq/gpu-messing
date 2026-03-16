#include "renderer.h"

Renderer::Renderer() {
}

RenderMode Renderer::getRenderMode() const {
	return renderMode;
}

void Renderer::setRenderMode(RenderMode renderMode) {
	this->renderMode = renderMode;
}