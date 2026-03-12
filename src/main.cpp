#include "utils/commongl.h"
#include "core/core.h"

const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 768;

int main() {
    // Engine only for WINdows openGL
    Core core(SCREEN_WIDTH, SCREEN_HEIGHT);

    if (!core.init()) {
        return -1;
    }

    core.run();

    return 0;
}