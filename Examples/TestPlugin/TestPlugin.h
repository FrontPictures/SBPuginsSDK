#ifndef TESTPLUGIN_H
#define TESTPLUGIN_H

#include "SBNode.h"
#include <string>
#include <memory>
#include <atomic>
#include <vector>

#ifdef _WIN32
#define PLUGIN_EXPORTS __declspec(dllexport)
#else
#define PLUGIN_EXPORTS __attribute__((visibility("default")))
#endif

extern "C" PLUGIN_EXPORTS SBNode *createSBPluginNode2(SBHost *host);

class TestShader;
class TestPluginImage;

class TestPlugin : public SBNode
{
public:
    TestPlugin(SBHost *host);
    ~TestPlugin() override;

    void process() override;

    void contextBuffersSwapped() override;

    void processAudio(int samples) override;

private:
    void processImage(const SBImageInfo &inputImage, bool recreateImage);
    void processMessages();
    void setCounter(std::string prefix, int counter);
    void emitState();

    SBParameter *mIncreaseCounterPar = nullptr;
    SBParameter *mInputImagePar = nullptr;
    SBParameter *mInitialValuePar = nullptr;
    SBParameter *mPrefixPar = nullptr;
    SBParameter *mInputMessagesPar = nullptr;
    SBParameter *mRecreateOutputImagePar = nullptr;

    bool mInited = false;

    int mCounter = 0;
    std::string mCounterString = "";
    std::string mPrefix;
    bool mGladInited = false;

    std::unique_ptr<TestShader> mShader;
    std::unique_ptr<TestShader> mShader2d;
    std::unique_ptr<TestPluginImage> mImage;
    SBImageInfo mOutputImageInfo;
    std::unique_ptr<TestPluginImage> mPrevImage;

    std::unique_ptr<TestPluginImage> mImage2d;
    SBImageInfo mOutputImageInfo2d;

    uint32_t mRenderedForChangeId = 0;
    int mSwapsCounter = 0;

    SBHost *mHost = nullptr;

    std::atomic<float> mAudioVolume = 1;
};

#endif // TESTPLUGIN_H
