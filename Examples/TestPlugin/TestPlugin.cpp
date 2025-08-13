#include "TestPlugin.h"
#include <cstdio>
#include "glad/gl.h"
#include "SDL2/SDL_video.h"
#include "TestShader.h"
#include "TestPluginImage.h"

SBNode *createSBPluginNode2(SBHost *host) { return new TestPlugin(host); }

TestPlugin::TestPlugin(SBHost *host) : mHost(host)
{
    mIncreaseCounterPar = mHost->addInputParameter(SBParameter::type_action, "Increase Counter");
    mInputImagePar = mHost->addInputParameter(SBParameter::type_image, "Input Image");
    mInitialValuePar = mHost->addInputParameter(SBParameter::type_int, "Initial value");
    mPrefixPar = mHost->addInputParameter(SBParameter::type_string, "Prefix");
    mInputMessagesPar = mHost->addInputParameter(SBParameter::type_messages, "Input Messages");
    mHost->addInputParameter(SBParameter::type_double, "Audio Volume",
                             {.doubleValue = mAudioVolume});
    mHost->addInputParameter(SBParameter::type_audio, "Audio Input");
    mRecreateOutputImagePar = mHost->addInputParameter(SBParameter::type_action,
                                                       "Recreate Output Image");
    mHost->addInputParameter(SBParameter::type_double, "Dummy", {.doubleValue = 2.5});

    mHost->addOutputParameter(SBParameter::type_int, "Counter", {.intValue = mCounter});
    mHost->addOutputParameter(SBParameter::type_string, "Counter String");
    mHost->addOutputParameter(SBParameter::type_image, "Output Image");
    mHost->addOutputParameter(SBParameter::type_image, "Output Image 2d");
    mHost->addOutputParameter(SBParameter::type_int, "Swaps Counter", {.intValue = mSwapsCounter});
    mHost->addOutputParameter(SBParameter::type_messages, "Output Plugin Events");
    mHost->addOutputParameter(SBParameter::type_messages, "Output Button Events");
    mHost->addOutputParameter(SBParameter::type_audio, "Audio Output");
}

TestPlugin::~TestPlugin() = default;

void TestPlugin::process()
{
    if (mPrefixPar->changed) {
        mPrefix = mPrefixPar->value.string;
        setCounter(mPrefix, mCounter);
        mHost->log("Prefix changed");
    }

    if (!mInited) {
        setCounter(mPrefix, mInitialValuePar->value.intValue);
        emitState();
        mInited = true;
        mHost->log("Test Plugin Initialized");
    }

    processMessages();

    if (mIncreaseCounterPar->value.action) {
        setCounter(mPrefix, mCounter + 1);
        emitState();
        mHost->log("Counter increased by input button parameter");
        mHost->pushOutputParameterMessageEvent("Output Plugin Events", "TestPluginEvent");
        mHost->pushOutputParameterMessageJson("Output Button Events",
                                              "[100,[\"TestButtonEvent\"]]");
    }
    while (int size = mHost->getNextInputParameterMessageSize("Input Messages")) {
        std::string msg;
        msg.resize(size);
        mHost->popNextInputParameterMessage("Input Messages", msg.data(), msg.size());
        mHost->log("Received a message:");
        mHost->log(msg.c_str());
        if (msg == "[100,[\"Button\"]]") {
            setCounter(mPrefix, mCounter + 1);
            emitState();
            mHost->log("Counter increased by input message event from a button");
        } else {
            mHost->logError("Unknown message");
        }
    }

    auto *volumePar = mHost->getInputParameter("Audio Volume");
    if (volumePar->changed) {
        mAudioVolume = volumePar->value.doubleValue;
    }

    mHost->setOutputParameter("Counter", {.intValue = mCounter});
    mHost->setOutputParameter("Counter String", {.string = mCounterString.c_str()});

    processImage(mInputImagePar->value.imageInfo, mRecreateOutputImagePar->value.action);
    mHost->setOutputParameter("Swaps Counter", {.intValue = mSwapsCounter});
}

void TestPlugin::contextBuffersSwapped() { mSwapsCounter++; }

void TestPlugin::processAudio(int samples)
{
    int channels = mHost->getInputAudioChannels("Audio Input");
    mHost->setOutputAudioChannels("Audio Output", channels);

    std::vector<float> buffer;
    buffer.resize(samples);
    for (int i = 0; i < channels; ++i) {
        mHost->readInputAudio("Audio Input", i, buffer.data(), buffer.size());
        for (auto &value : buffer) {
            value *= mAudioVolume;
        }
        mHost->writeOutputAudio("Audio Output", i, buffer.data(), buffer.size());
    }
}

void TestPlugin::processImage(const SBImageInfo &inputImage, bool recreateImage)
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
    if (mImage && recreateImage) {
        mPrevImage = std::move(mImage);
        mPrevImage->renderBegin();
        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT);
        mPrevImage->renderEnd();
        mImage = {};
        mImage2d = {};
    }

    if (mImage
        && (mImage->getWidth() != inputImage.width || mImage->getHeight() != inputImage.height)) {
        mImage = {};
        mImage2d = {};
        mOutputImageInfo = {};
    }
    if (!mImage) {
        mHost->disableWatchdogTimer(30);

        mImage = std::unique_ptr<TestPluginImage>(
            new TestPluginImage(inputImage.width, inputImage.height, GL_TEXTURE_RECTANGLE));
        mOutputImageInfo.textureId = mImage->getTextureId();
        mOutputImageInfo.width = mImage->getWidth();
        mOutputImageInfo.height = mImage->getHeight();
        mOutputImageInfo.numChannels = 4;
        mOutputImageInfo.dataType = SBImageInfo::type_uint8;
        mOutputImageInfo.textureType = SBImageInfo::texture_rectangle;
        mOutputImageInfo.renderCount = 0;

        mImage2d = std::unique_ptr<TestPluginImage>(
            new TestPluginImage(inputImage.width, inputImage.height, GL_TEXTURE_2D));
        mOutputImageInfo2d = mOutputImageInfo;
        mOutputImageInfo2d.textureId = mImage2d->getTextureId();
        mOutputImageInfo2d.textureType = SBImageInfo::texture_2d;

        mRenderedForChangeId = 0;
    }
    if (inputImage.renderCount != mRenderedForChangeId) {
        mRenderedForChangeId = inputImage.renderCount;

        mImage->renderBegin();
        mShader->render(inputImage.textureId);
        mImage->renderEnd();
        mOutputImageInfo.renderCount++;

        mImage2d->renderBegin();
        mShader->render(inputImage.textureId);
        mImage2d->renderEnd();
        mOutputImageInfo2d.renderCount++;
    }
    mHost->setOutputParameter("Output Image", {.imageInfo = mOutputImageInfo});
    mHost->setOutputParameter("Output Image 2d", {.imageInfo = mOutputImageInfo2d});
}

void TestPlugin::processMessages()
{
    while (int size = mHost->getNextMessengerMessageSize()) {
        std::string msgStr;
        msgStr.resize(size);
        mHost->popNextMessengerMessage(msgStr.data(), msgStr.size());
        if (msgStr == "GetState") {
            emitState();
            continue;
        }
        if (msgStr == "Increment") {
            setCounter(mPrefix, mCounter + 1);
            emitState();
        }
    }
}

void TestPlugin::setCounter(std::string prefix, int counter)
{
    mCounter = counter;
    mCounterString = prefix + std::to_string(mCounter);
}

void TestPlugin::emitState()
{
    mHost->sendMessengerMessage(mCounterString.c_str(), mCounterString.size());
}
