#pragma once

#include "cores/back_end.h"
#include <memory>

constexpr int SCREEN_WIDTH = 1280;
constexpr int SCREEN_HEIGHT = 768;

int main() {
    auto backEnd = std::make_unique<BackEnd>(GraphicsAPI::OPEN_GL, SCREEN_WIDTH, SCREEN_HEIGHT);

    if (!backEnd->init()) {
        return -1;
    }

    backEnd->run();

    return 0;
}