#include "frame_buffer.h"

FrameBuffer::FrameBuffer(FrameBuffer&& other) noexcept {
}

FrameBuffer& FrameBuffer::operator=(FrameBuffer&& other) noexcept {
    if (this != &other) {
    }
    return *this;
}

FrameBuffer::FrameBuffer() {
}

FrameBuffer::~FrameBuffer() {
}