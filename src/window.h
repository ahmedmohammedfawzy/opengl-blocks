#pragma once

#include "cglm/struct.h"
#include "gfx.h"
typedef void (*FWindow)(void);

typedef struct Button {
    bool down;
} Button;

typedef struct Keyboard {
    Button keys[GLFW_KEY_LAST];
} Keyboard;

typedef struct Mouse {
    vec2s position, delta;
} Mouse;

typedef struct Window {
    GLFWwindow* handle;
    ivec2s size;
    FWindow init, render;
    Keyboard keyboard;
    Mouse mouse;
    float deltaTime, lastFrame;
} Window;

extern Window window;

void window_loop(void);
void window_create(FWindow init, FWindow render);
