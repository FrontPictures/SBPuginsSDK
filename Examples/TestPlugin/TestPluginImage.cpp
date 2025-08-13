#include "TestPluginImage.h"
#include <cstdio>

TestPluginImage::TestPluginImage(uint32_t width, uint32_t height, GLenum glTextureType)
    : mWidth(width), mHeight(height)
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
    glBindTexture(glTextureType, mTexture);

    glTexParameteri(glTextureType, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(glTextureType, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(glTextureType, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(glTextureType, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(glTextureType, 0, GL_RGBA8, mWidth, mHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                 0);

    glGenFramebuffers(1, &mFbo);
    if (mFbo == 0) {
        printf("Error: fbo not created\n");
        return;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, mFbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, glTextureType, mTexture,
                           0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    if (auto err = glGetError()) {
        printf("OpenGL error: 0x%04x\n", int(err));
        return;
    }
}

TestPluginImage::~TestPluginImage()
{
    if (mFbo != 0) {
        glDeleteFramebuffers(1, &mFbo);
    }
    if (mTexture != 0) {
        glDeleteTextures(1, &mTexture);
    }
}

void TestPluginImage::renderBegin()
{
    glBindFramebuffer(GL_FRAMEBUFFER, mFbo);
    glViewport(0, 0, mWidth, mHeight);
}

void TestPluginImage::renderEnd() { glBindFramebuffer(GL_FRAMEBUFFER, 0); }
