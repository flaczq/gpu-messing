#pragma once

#include "graphics_types.hpp"
#include <memory>

class FrameBuffer {
public:
	FrameBuffer(FrameBuffer&& other) noexcept;
	FrameBuffer& operator=(FrameBuffer&& other) noexcept;
	FrameBuffer(const FrameBuffer&) = delete;
	FrameBuffer& operator=(const FrameBuffer&) = delete;
	FrameBuffer(std::shared_ptr<Texture> texture);
	~FrameBuffer();

	void bind();
	void unbind();

private:
	unsigned int m_FBO{};
	unsigned int m_RBO{};
	std::shared_ptr<Texture> m_texture{};

	void _setupFrameBuffer();
};