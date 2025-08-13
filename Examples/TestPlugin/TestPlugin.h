#ifndef TESTPLUGIN_H
#define TESTPLUGIN_H

#include "SBNode.h"
#include <string>
#include <memory>

#ifdef _WIN32
#define PLUGIN_EXPORTS __declspec(dllexport)
#else
#define PLUGIN_EXPORTS __attribute__((visibility("default")))
#endif

extern "C" PLUGIN_EXPORTS SBNode *createSBPluginNode();

class TestShader;
class TestImage;

class TestPlugin : public SBNode
{
public:
    TestPlugin();
    ~TestPlugin() override;

    void setSBHost(SBHost *host) override { mHost = host; }
    void setSBMessanger(SBMessanger *messanger) override { mMessanger = messanger; }

    size_t getNumberOfInputs() const override;
    void fillInputInfo(SBParameters inputs) const override;

    size_t getNumberOfOutputs() const override;
    void fillOutputInfo(SBParameters outputs) const override;

    void process(SBParameters inputs, SBParameters outputs) override;

    void contextBuffersSwapped() override;

private:
    void processImage(const SBImageInfo &inputImage, SBImageInfo &outputImage);
    void processMessages();
    void setCounter(int counter);
    void emitState();

    bool mInited = false;

    int mCounter = 0;
    std::string mCounterString = "";
    bool mGladInited = false;

    std::unique_ptr<TestShader> mShader;
    std::unique_ptr<TestImage> mImage;
    uint32_t mRenderedForChangeId = 0;
    int mSwapsCounter = 0;

    SBHost *mHost = nullptr;
    SBMessanger *mMessanger = nullptr;
};

#endif // TESTPLUGIN_H
