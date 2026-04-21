#pragma once

#define _CRTDBG_MAP_ALLOC
#include "cores/back_end.h"
#include <crtdbg.h>
#include <iostream>
#include <memory>
#include <stdlib.h>

constexpr int SCREEN_WIDTH = 1280;
constexpr int SCREEN_HEIGHT = 768;

int main() {
    // check for memory leaks
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    // no printf sync -> make std::cout faster
    std::ios_base::sync_with_stdio(false);

    auto backEnd = std::make_unique<BackEnd>(GraphicsAPI::OPEN_GL, SCREEN_WIDTH, SCREEN_HEIGHT);

    if (!backEnd->init()) {
        return -1;
    }

    backEnd->run();

    return 0;
}