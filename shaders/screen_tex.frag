#version 330 core

struct Material {
	sampler2D diffuse;
};

out vec4 FragColor;

in vec2 TexCoords;

uniform Material material;

vec4 inverse(vec3 col) {
    return vec4(vec3(1.0 - col), 1.0);
}
vec4 grayscale() {
    float average = 0.2126 * FragColor.r + 0.7152 * FragColor.g + 0.0722 * FragColor.b;
    return vec4(average, average, average, 1.0);
}
vec4 kernelBlurEdges(int effect) {
    const float offset = 1.0 / 300.0;
    vec2 offsets[9] = vec2[](
        vec2(-offset,  offset),
        vec2( 0.0f,    offset),
        vec2( offset,  offset),
        vec2(-offset,  0.0f),
        vec2( 0.0f,    0.0f),
        vec2( offset,  0.0f),
        vec2(-offset, -offset),
        vec2( 0.0f,   -offset),
        vec2( offset, -offset)
    );
    float kernel[9] = float[](
        -1, -1, -1,
        -1,  9, -1,
        -1, -1, -1
    );
    float blur[9] = float[](
        1.0 / 16, 2.0 / 16, 1.0 / 16,
        2.0 / 16, 4.0 / 16, 2.0 / 16,
        1.0 / 16, 2.0 / 16, 1.0 / 16  
    );
    float edges[9] = float[](
        1, 1, 1,
        1, -8, 1,
        1, 1, 1
    );
    float eff[9];
    if (effect == 0) {
        eff = kernel;
    } else if (effect == 1) {
        eff = blur;
    } else if (effect == 2) {
        eff = edges;
    }
    vec3 sampleTex[9];
    for (int i = 0; i < 9; i++) {
        sampleTex[i] = vec3(texture(material.diffuse, TexCoords.st + offsets[i]));
    }
    vec3 col = vec3(0.0);
    for (int i = 0; i < 9; i++) {
        col += sampleTex[i] * eff[i];
    }
    return vec4(col, 1.0);
}

void main() {
    vec3 color = texture(material.diffuse, TexCoords).rgb;
    //FragColor = vec4(color, 1.0);
    //FragColor = inverse(color);
    //FragColor = grayscale();
    FragColor = kernelBlurEdges(2);
}