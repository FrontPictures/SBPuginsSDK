#ifndef TESTIMAGE_H
#define TESTIMAGE_H

#include <cstdint>
#include "glad/gl.h"

class TestImage
{
public:
    TestImage(uint32_t width, uint32_t height);
    ~TestImage();

    uint32_t getWidth() const { return mWidth; }
    uint32_t getHeight() const { return mHeight; }

    uint32_t getTextureId() const { return mTexture; }

    void renderBegin();
    void renderEnd();

private:
    uint32_t mWidth = 0;
    uint32_t mHeight = 0;
    GLuint mTexture = 0;
    GLuint mFbo = 0;
};

#endif // TESTIMAGE_H
