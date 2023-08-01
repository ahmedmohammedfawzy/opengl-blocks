#include "window.h"
#include "GLFW/glfw3.h"
#include <stdio.h>

// global window object
Window window;

bool ignoreMouse = false;


static void _error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

void _cursor_enter_callback(GLFWwindow* handle, int entered)
{
    if (entered)
        ignoreMouse = true;
}


static void _mouse_callback(GLFWwindow* handle, double xpos, double ypos)
{
    if (ignoreMouse) 
        ignoreMouse = false;
    else 
        window.mouse.delta = glms_vec2_sub((vec2s){{xpos, ypos}}, window.mouse.position);

    window.mouse.position = (vec2s) {{xpos, ypos}};
}

static void _key_callback(GLFWwindow *handle, int key, int scancode, int action, int mods) {
    if (key < 0) {
        return;
    }

    switch (action) {
        case GLFW_PRESS:
            window.keyboard.keys[key].down = true;
            break;
        case GLFW_RELEASE:
            window.keyboard.keys[key].down = false;
            break;
        default:
            break;
    }
}

static void _size_callback(GLFWwindow* handle, int width, int height)
{
    glViewport(0, 0, width, height);
    window.size = (ivec2s) {{width, height}};
}
 
void window_create(FWindow init, FWindow render) {
    window.init = init;
    window.render = render;

    glfwSetErrorCallback(_error_callback);
 
    if (!glfwInit())
        exit(EXIT_FAILURE);
 
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
 
    window.size = (ivec2s) {{1280, 720}};
    window.handle = 
        glfwCreateWindow(window.size.x, window.size.y, "Minecraft", NULL, NULL);

    if (window.handle == NULL)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(window.handle);
 
    glfwSetFramebufferSizeCallback(window.handle, _size_callback);
    glfwSetCursorPosCallback(window.handle, _mouse_callback);  
    glfwSetKeyCallback(window.handle, _key_callback);
    glfwSetCursorEnterCallback(window.handle, _cursor_enter_callback);
 
    if (!gladLoadGL(glfwGetProcAddress)) {
        fprintf(stderr, "%s",  "error initializing GLAD\n");
        glfwTerminate();
        exit(1);
    }

    glfwSetInputMode(window.handle, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSwapInterval(1);
}

void window_loop(void) {
    window.init();

    while (!glfwWindowShouldClose(window.handle))
    {
        float currentFrame = glfwGetTime();
        window.deltaTime = currentFrame - window.lastFrame;
        window.lastFrame = currentFrame;

        window.render();
        glfwSwapBuffers(window.handle);
        glfwPollEvents();
    }

    glfwDestroyWindow(window.handle);
 
    glfwTerminate();
    exit(EXIT_SUCCESS);
}
