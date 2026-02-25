#include "cpu_main.h"

const int WIDTH = 800;
const int HEIGHT = 600;

// Nasz bufor: szerokość * wysokość * 3 kanały (RGB)
std::vector<unsigned char> cpuFrameBuffer(WIDTH* HEIGHT * 3, 0);

// Funkcja ustawiająca kolor konkretnego piksela
void setPixel(int x, int y, unsigned char r, unsigned char g, unsigned char b) {
    if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT) {
        int index = (y * WIDTH + x) * 3;
        cpuFrameBuffer[index] = r;
        cpuFrameBuffer[index + 1] = g;
        cpuFrameBuffer[index + 2] = b;
    }
}

void renderTriangleCPU() {
    // Bardzo uproszczony algorytm wypełniania prostokąta (zamiast trójkąta dla testu FPS)
    for (int y = 200; y < 400; y++) {
        for (int x = 300; x < 500; x++) {
            setPixel(x, y, 255, 128, 0); // Pomarańczowy kwadrat liczony przez CPU
        }
    }
}

void main() {
    // W pętli głównej (run):
    renderTriangleCPU(); // Liczymy piksele na procesorze

    // Aktualizujemy teksturę danymi z RAM
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WIDTH, HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, cpuFrameBuffer.data());

    // Renderujemy jeden wielki prostokąt na cały ekran, na który nałożona jest ta tekstura
    renderFullScreenQuad();
}

unsigned int quadVAO = 0;
unsigned int quadVBO;

void initFullScreenQuad() {
    float quadVertices[] = {
        // pozycje (x, y) | współrzędne tekstury (u, v)
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };

    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);

    // Pozycja (Location 0 w shaderze)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    // Tekstura (Location 1 w shaderze)
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
}

void renderFullScreenQuad() {
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}