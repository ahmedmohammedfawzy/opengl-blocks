#include "cglm/call/cam.h"
#include "cglm/struct/affine-pre.h"
#include "cglm/types.h"
#include "cglm/call.h"

#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "cglm/call.h"
#include <stdlib.h>
#include <stdio.h>
#include "shader.h"
 
int win_width = 640;
int win_height = 480;

float vertices[] = {
    0.5f,  0.5f, 0.0f,  // top right
    0.5f, -0.5f, 0.0f,  // bottom right
    -0.5f, -0.5f, 0.0f,  // bottom left
    -0.5f,  0.5f, 0.0f   // top left 
};
unsigned int indices[] = {  // note that we start from 0!
    0, 1, 3,  // first Triangle
    1, 2, 3   // second Triangle
};
void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    win_width = width;
    win_height = height;
    glViewport(0, 0, width, height);
}
 
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}
 
int main(void)
{
    GLFWwindow* window;
 
    glfwSetErrorCallback(error_callback);
 
    if (!glfwInit())
        exit(EXIT_FAILURE);
 
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
 
    window = glfwCreateWindow(win_width, win_height, "Minecraft", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
 
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);
 
    glfwMakeContextCurrent(window);
    gladLoadGL(glfwGetProcAddress);
    glfwSwapInterval(1);

    // OpenGL
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    unsigned int EBO;
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    Shader ourShader = shader_create("res/shaders/basic.vs", "res/shaders/basic.fs");

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Render loop
    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        shader_use(ourShader);

        // Create transformations
        mat4s model = GLMS_MAT4_IDENTITY_INIT;
        mat4s view = GLMS_MAT4_IDENTITY_INIT;
        mat4s projection = GLMS_MAT4_IDENTITY_INIT;

        model = glms_rotate(model, glm_rad(-55.0f), (vec3s){{1.0f, 0.0f, 0.0f}});
        view = glms_translate(view, (vec3s){{0.0f, 0.0f, -3.0f}});
        glmc_perspective(glm_rad(45.0f), 
                (float)win_width / (float)win_height, 0.1f, 100.0f, projection.raw);


        shader_uniform_mat4(ourShader, "model", model);
        shader_uniform_mat4(ourShader, "view", view);
        shader_uniform_mat4(ourShader, "projection", projection);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
 
    glfwDestroyWindow(window);
 
    glfwTerminate();
    exit(EXIT_SUCCESS);
}
