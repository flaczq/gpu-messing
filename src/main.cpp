#include "utils/config.h"
#include "core/back_end.h"

const int s_width = 1280;
const int s_height = 768;

int main() {
    auto backEnd = std::make_unique<BackEnd>(GraphicsAPI::OPEN_GL, s_width, s_height);

    if (!backEnd->init()) {
        return -1;
    }

    backEnd->run();

    return 0;
}