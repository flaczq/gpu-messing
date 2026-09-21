#pragma once

#define _CRTDBG_MAP_ALLOC
#include "api/backend.h"
#include <crtdbg.h>
#include <iostream>
#include <memory>
#include <stdlib.h>

constexpr unsigned int SCREEN_WIDTH = 1280;
constexpr unsigned int SCREEN_HEIGHT = 768;

int main() {
    // check for memory leaks
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    // no printf sync -> make std::cout faster
    std::ios_base::sync_with_stdio(false);

    std::unique_ptr<BackEnd> backEnd = BackEnd::create(GraphicsAPI::OPENGL);
    if (!backEnd->init(SCREEN_WIDTH, SCREEN_HEIGHT)) {
        return -1;
    }

    backEnd->run();

    return 0;
}