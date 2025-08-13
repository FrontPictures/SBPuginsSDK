#include "TestImage.h"
#include <cstdio>

TestImage::TestImage(uint32_t width, uint32_t height) : mWidth(width), mHeight(height)
{
    if (mWidth == 0 || mHeight == 0) {
        printf("Error: format is not valid\n");
        return;
    }
    glGenTextures(1, &mTexture);
    if (mTexture == 0) {
        printf("Error: texture not allocated\n");
        return;
    }
    glBindTexture(GL_TEXTURE_RECTANGLE, mTexture);

    glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGBA8, mWidth, mHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                 0);

    glGenFramebuffers(1, &mFbo);
    if (mFbo == 0) {
        printf("Error: fbo not created\n");
        return;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, mFbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_RECTANGLE, mTexture,
                           0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    if (auto err = glGetError()) {
        printf("OpenGL error: 0x%04x", int(err));
        return;
    }
}

TestImage::~TestImage()
{
    if (mFbo != 0) {
        glDeleteFramebuffers(1, &mFbo);
    }
    if (mTexture != 0) {
        glDeleteTextures(1, &mTexture);
    }
}

void TestImage::renderBegin()
{
    glBindFramebuffer(GL_FRAMEBUFFER, mFbo);
    glViewport(0, 0, mWidth, mHeight);
}

void TestImage::renderEnd() { glBindFramebuffer(GL_FRAMEBUFFER, 0); }
