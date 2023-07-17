#include "shader.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void _log_and_fail(
    GLint handle, const char* adverb, const char* path,
    void (*getlog)(GLuint, GLsizei, GLsizei*, GLchar*),
    void (*getiv)(GLuint, GLenum, GLint*)) 
{
    GLint loglen;
    getiv(handle, GL_INFO_LOG_LENGTH, &loglen);

    char *logtext = calloc(1, loglen);
    getlog(handle, loglen, NULL, logtext);
    fprintf(stderr, "Error %s shader at %s:\n%s", adverb, path, logtext);

    free(logtext);
    exit(1);
}

static GLint _compile(char *path, GLenum type) {
    FILE* f;
    char* text;
    long len;

    f = fopen(path, "rb");
    if (f == NULL) {
        fprintf(stderr, "error loading shader at %s\n", path);
        exit(1);
    }

    fseek(f, 0, SEEK_END);
    len = ftell(f);
    assert(len > 0);
    fseek(f, 0, SEEK_SET);
    text = calloc(1, len);
    assert(text != NULL);
    fread(text, 1, len, f);
    assert(strlen(text) > 0);
    fclose(f);

    GLuint handle = glCreateShader(type);
    glShaderSource(handle, 1, (const GLchar* const*) &text, (const GLint*) &len);
    glCompileShader(handle);

    GLint compiled;
    glGetShaderiv(handle, GL_COMPILE_STATUS, &compiled);

    // Check OpenGL logs if compilation failed
    if (compiled == 0) {
        _log_and_fail(handle, "compiling", path, glGetShaderInfoLog, glGetShaderiv);
    }

    free(text);
    return handle;
}

Shader shader_create(char *vs_path, char *fs_path) {
    Shader self;
    GLuint vs = _compile(vs_path, GL_VERTEX_SHADER);
    GLuint fs = _compile(fs_path, GL_FRAGMENT_SHADER);

    self.handle = glCreateProgram();
    glAttachShader(self.handle, vs);
    glAttachShader(self.handle, fs);
    glLinkProgram(self.handle);

        // Check link status
    GLint linked;
    glGetProgramiv(self.handle, GL_LINK_STATUS, &linked);

    if (linked == 0) {
        char buf[512];
        snprintf(buf, 512, "[%s, %s]", vs_path, fs_path);
        _log_and_fail(self.handle, "linking", buf, glGetProgramInfoLog, glGetProgramiv);
    }

    // delete the shaders as they're linked into our program now and no longer necessary
    glDeleteShader(vs);
    glDeleteShader(fs);

    return self;
}

void shader_use(Shader self) {
    glUseProgram(self.handle);
}

void shader_destroy(Shader self) {
    // todo
}

void shader_uniform_vec4(Shader self, char* name, vec4s v) {
    int uniform_location = glGetUniformLocation(self.handle, name);
    glUniform4f(uniform_location, v.r, v.g, v.b, v.a);
}

void shader_uniform_mat4(Shader self, char *name, mat4s m) {
    int uniform_location = glGetUniformLocation(self.handle, name);
    glUniformMatrix4fv(uniform_location, 1, GL_FALSE, &m.raw[0][0]);
}
