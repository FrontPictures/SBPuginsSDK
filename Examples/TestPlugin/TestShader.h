#ifndef TESTSHADER_H
#define TESTSHADER_H

#include "glad/gl.h"

class TestShader
{
public:
    TestShader();
    ~TestShader();

    void render(GLuint textureId);

private:
    GLuint mVertexShader=0;
    GLuint mFragmentShader=0;

    GLuint mShaderProgram=0;

    GLint mLocation=-1;

    GLuint mVAO=0;    // Vertex Array Object
    GLuint mVBO=0;    // Vertex Buffer Object

    GLsizei mVertsCount=0;
};

#endif // SHADER_H
