#include "commongl.h"
#include "engine.h"

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

int main() {
    Engine engine(SCREEN_WIDTH, SCREEN_HEIGHT);

    if (!engine.init()) {
        return -1;
    }

    engine.run();

    return 0;
}