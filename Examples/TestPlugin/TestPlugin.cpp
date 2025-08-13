#include "TestPlugin.h"
#include <cstdio>
#include "glad/gl.h"
#include "SDL2/SDL_video.h"
#include "TestShader.h"
#include "TestImage.h"

SBNode *createSBPluginNode() { return new TestPlugin(); }

TestPlugin::TestPlugin() = default;

TestPlugin::~TestPlugin() = default;

size_t TestPlugin::getNumberOfInputs() const { return 3; }

void TestPlugin::fillInputInfo(SBParameters inputs) const
{
    if (inputs.count != getNumberOfInputs() || !inputs.parameters) {
        printf("invalid count\n");
        return;
    }
    inputs.parameters[0].type = SBParameter::type_action;
    inputs.parameters[0].name = "Increase Counter";

    inputs.parameters[1].type = SBParameter::type_image;
    inputs.parameters[1].name = "Input Image";

    inputs.parameters[2].type = SBParameter::type_int;
    inputs.parameters[2].name = "Initial value";
}

size_t TestPlugin::getNumberOfOutputs() const { return 4; }

void TestPlugin::fillOutputInfo(SBParameters outputs) const
{
    if (outputs.count != 4 || !outputs.parameters) {
        printf("invalid count\n");
        return;
    }
    outputs.parameters[0].type = SBParameter::type_int;
    outputs.parameters[0].name = "Counter";
    outputs.parameters[0].value.intValue = mCounter;

    outputs.parameters[1].type = SBParameter::type_string;
    outputs.parameters[1].name = "Counter String";
    outputs.parameters[1].value.string = mCounterString.c_str();

    outputs.parameters[2].type = SBParameter::type_image;
    outputs.parameters[2].name = "Output Image";

    outputs.parameters[3].type = SBParameter::type_int;
    outputs.parameters[3].name = "Swaps Counter";
    outputs.parameters[3].value.intValue = mSwapsCounter;
}

void TestPlugin::process(SBParameters inputs, SBParameters outputs)
{
    if (inputs.count != getNumberOfInputs() || !inputs.parameters
        || outputs.count != getNumberOfOutputs() || !outputs.parameters) {
        printf("invalid inputs/outputs\n");
        return;
    }
    if (!mInited) {
        setCounter(inputs.parameters[2].value.intValue);
        emitState();
        mInited = true;
    }

    processMessages();

    if (inputs.parameters[0].value.action) {
        setCounter(mCounter + 1);
        emitState();
    }
    outputs.parameters[0].value.intValue = mCounter;
    outputs.parameters[1].value.string = mCounterString.c_str();

    processImage(inputs.parameters[1].value.imageInfo, outputs.parameters[2].value.imageInfo);
    outputs.parameters[3].value.intValue = mSwapsCounter;    
}

void TestPlugin::contextBuffersSwapped()
{
    mSwapsCounter++;
}

void TestPlugin::processImage(const SBImageInfo &inputImage, SBImageInfo &outputImage)
{
    if (!mGladInited) {
        if (!gladLoadGL((GLADloadfunc)SDL_GL_GetProcAddress)) {
            printf("gladLoadGL failed\n");
            return;
        }
        mGladInited = true;
    }
    if (inputImage.textureId == 0) {
        return;
    }
    if (inputImage.textureType != SBImageInfo::texture_rectangle) {
        printf("Error: unsupported input texture type\n");
        return;
    }
    if (!mShader) {
        mShader = std::unique_ptr<TestShader>(new TestShader());
    }
    if (mImage
        && (mImage->getWidth() != inputImage.width || mImage->getHeight() != inputImage.height)) {
        mImage = {};
        outputImage = {};
    }
    if (!mImage) {
        if (!mHost) {
            printf("Error: no host\n");
            return;
        }
        mHost->disableWatchdogTimer(30);
        mImage = std::unique_ptr<TestImage>(new TestImage(inputImage.width, inputImage.height));
        outputImage.textureId = mImage->getTextureId();
        outputImage.width = mImage->getWidth();
        outputImage.height = mImage->getHeight();
        outputImage.numChannels = 4;
        outputImage.dataType = SBImageInfo::type_uint8;
        outputImage.textureType = SBImageInfo::texture_rectangle;
        outputImage.renderCount = 0;
        mRenderedForChangeId = 0;
    }
    if (inputImage.renderCount == mRenderedForChangeId) {
        return;
    }
    mRenderedForChangeId = inputImage.renderCount;
    mImage->renderBegin();
    mShader->render(inputImage.textureId);
    mImage->renderEnd();
    outputImage.renderCount++;
}

void TestPlugin::processMessages()
{
    if (!mMessanger) {
        printf("Error: No messanger\n");
        return;
    }
    while (SBMessage msg = mMessanger->nextMessage()) {
        std::string msgStr(msg.ptr, msg.size);
        if (msgStr == "GetState") {
            emitState();
            continue;
        }
        if (msgStr == "Increment") {
            setCounter(mCounter + 1);
            emitState();
        }
    }
}

void TestPlugin::setCounter(int counter)
{
    mCounter = counter;
    mCounterString = std::to_string(mCounter);
}

void TestPlugin::emitState()
{
    mMessanger->sendMessage({mCounterString.c_str(), mCounterString.size()});
}
