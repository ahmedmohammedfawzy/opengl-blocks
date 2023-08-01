#include "cglm/types.h"
#include "cglm/call.h"

#include <stdlib.h>
#include <stdio.h>
#include "shader.h"
#include "window.h"
#include "util/types.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
 
vec3s cameraPos   = (vec3s){{0.0f, 0.0f,  3.0f}};
vec3s cameraFront = (vec3s){{0.0f, 0.0f, -1.0f}};
vec3s cameraUp    = (vec3s){{0.0f, 1.0f,  0.0f}};

f32 yaw   = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
f32 pitch =  0.0f;
f32 lastX =  800.0f / 2.0;
f32 lastY =  600.0 / 2.0;

f32 vertices[] = {
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
};

void processInput(void)
{
    if (window.keyboard.keys[GLFW_KEY_ESCAPE].down)
        glfwSetWindowShouldClose(window.handle, true);

    const f32 cameraSpeed = 2.5f * window.deltaTime; // adjust accordingly
    if (window.keyboard.keys[GLFW_KEY_W].down) 
        cameraPos = glms_vec3_add(cameraPos, glms_vec3_scale(cameraFront, cameraSpeed));
    if (window.keyboard.keys[GLFW_KEY_S].down)
        cameraPos = glms_vec3_sub(cameraPos, glms_vec3_scale(cameraFront, cameraSpeed));
    if (window.keyboard.keys[GLFW_KEY_A].down) {
        vec3s cameraRight = glms_normalize(glms_cross(cameraFront, cameraUp));
        cameraPos = glms_vec3_sub(cameraPos, glms_vec3_scale(cameraRight, cameraSpeed));
    }
    if (window.keyboard.keys[GLFW_KEY_D].down) {
        vec3s cameraRight = glms_normalize(glms_cross(cameraFront, cameraUp));
        cameraPos = glms_vec3_add(cameraPos, glms_vec3_scale(cameraRight, cameraSpeed));
    }

    f32 xoffset = window.mouse.delta.x;
    f32 yoffset = -window.mouse.delta.y; 
    window.mouse.delta.x = 0;
    window.mouse.delta.y = 0;


    f32 sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw   += xoffset;
    pitch += yoffset;

    if(pitch > 89.0f)
        pitch = 89.0f;
    if(pitch < -89.0f)
        pitch = -89.0f;

    vec3s direction;
    direction.x = cos(glm_rad(yaw)) * cos(glm_rad(pitch));
    direction.y = sin(glm_rad(pitch));
    direction.z = sin(glm_rad(yaw)) * cos(glm_rad(pitch));
    cameraFront = glms_normalize(direction);
}

u32 VBO, VAO, texture;
Shader ourShader;

void init(void) {
    glEnable(GL_DEPTH_TEST);

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    ourShader = shader_create("res/shaders/basic.vs", "res/shaders/basic.fs");

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // texture attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3* sizeof(float)));
    glEnableVertexAttribArray(1);


    // load and create a texture 
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    int width, height, nrChannels;
    u8 *data = stbi_load("res/images/wall.jpg", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        printf("Failed to load texture");
    }
    stbi_image_free(data);

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

void render(void) {
    processInput();
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shader_use(ourShader);

    // Create transformations
    mat4s view = GLMS_MAT4_IDENTITY_INIT;
    mat4s projection = GLMS_MAT4_IDENTITY_INIT;

    view = glms_lookat(cameraPos, glms_vec3_add(cameraPos, cameraFront), cameraUp);

    projection = glms_perspective(
            glm_rad(45.0f), 
            (f32)window.size.x / (f32)window.size.y, 0.1f, 100.0f);

    shader_uniform_mat4(ourShader, "view", view);
    shader_uniform_mat4(ourShader, "projection", projection);

    glBindVertexArray(VAO);
    vec3s cubePositions[] = {
        {{ 0.0f,  0.0f,  0.0f }},
        {{ 2.0f,  5.0f, -15.0f}},
        {{-1.5f, -2.2f, -2.5f }},
        {{-3.8f, -2.0f, -12.3f}},
        {{ 2.4f, -0.4f, -3.5f }},
        {{-1.7f,  3.0f, -7.5f }},
        {{ 1.3f, -2.0f, -2.5f }},
        {{ 1.5f,  2.0f, -2.5f }},
        {{ 1.5f,  0.2f, -1.5f }},
        {{-1.3f,  1.0f, -1.5f }}
    };

    for(u8 i = 0; i < sizeof(cubePositions)/sizeof(cubePositions[0]); i++)
    {
        mat4s model = GLMS_MAT4_IDENTITY_INIT;
        model = glms_translate(model, cubePositions[i]);

        shader_uniform_mat4(ourShader, "model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
    glBindVertexArray(0);
}

 
int main(void)
{
    window_create(init, render);
    window_loop();
}
