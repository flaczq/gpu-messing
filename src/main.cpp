#include "utils/commongl.h"
#include "engine/engine.h"

const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 768;

int main() {
    // only for WINdows openGL
    Engine engine(SCREEN_WIDTH, SCREEN_HEIGHT);

    if (!engine.init()) {
        return -1;
    }

    engine.run();

    return 0;
}