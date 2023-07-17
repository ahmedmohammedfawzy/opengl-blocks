#pragma once
#include "cglm/types-struct.h"
#include "glad/gl.h"
#include "cglm/struct.h"

typedef struct Shader {
    GLuint handle;
} Shader;

Shader shader_create(char* vs_path, char* fs_path);
void shader_use(Shader self);
void shader_destroy(Shader self);
void shader_uniform_vec4(Shader self, char* name, vec4s v);
void shader_uniform_mat4(Shader self,char* name, mat4s m);
