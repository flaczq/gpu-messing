#pragma once

class FrameBuffer {
public:
	FrameBuffer(FrameBuffer&& other) noexcept;
	FrameBuffer& operator=(FrameBuffer&& other) noexcept;
	FrameBuffer(const FrameBuffer&) = delete;
	FrameBuffer& operator=(const FrameBuffer&) = delete;
	FrameBuffer();
	~FrameBuffer();

private:
	unsigned int m_framebuffer{};
	unsigned int m_fbTexture{};
	unsigned int m_quadVAO{};
};