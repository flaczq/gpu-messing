#pragma once

#include "graphics_types.hpp"
#include <memory>
#include <vector>

class FrameBuffer {
public:
	FrameBuffer(FrameBuffer&& other) noexcept;
	FrameBuffer& operator=(FrameBuffer&& other) noexcept;
	FrameBuffer(const FrameBuffer&) = delete;
	FrameBuffer& operator=(const FrameBuffer&) = delete;
	FrameBuffer(std::vector<Vertex>& vertices, std::shared_ptr<Texture> texture);
	~FrameBuffer();

private:
	unsigned int m_FBO{};
	unsigned int m_RBO{};
	unsigned int m_quadVAO{};
	unsigned int m_quadVBO{};
	std::shared_ptr<Texture> m_texture{};

	void _setupFrameBuffer();
};