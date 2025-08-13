#ifndef SBNODE_H
#define SBNODE_H

/* Screenberry plugin consists of two libraries.
Server plugin is loaded by the server. Panel plugin should be placed on panel side and is
loaded by ScreenberryPanel at runtime.
Path for plugins is in binary folder (near Screenberry.exe):
Plugins/<Name>/<Name>.dll
Plugins/<Name>/<Name>Widget.dll

Each plugin library exports one method to create node or widget:

#ifdef _WIN32
#define PLUGIN_EXPORTS __declspec(dllexport)
#else
#define PLUGIN_EXPORTS __attribute__((visibility("default")))
#endif

// for server node plugin:
extern "C" PLUGIN_EXPORTS SBNode *createSBPluginNode2(SBHost *host);

// for panel widget plugin:
extern "C" PLUGIN_EXPORTS QWidget *createSBPluginWidget2(SBMessenger *messenger,
                                                        const SBClientInfo &info);

*/

#include <cstdint>

struct SBImageInfo
{
    enum DataType { type_invalid, type_uint8, type_float32 };
    enum TextureType { texture_invalid, texture_2d, texture_rectangle };

    uint32_t textureId = 0;
    uint32_t renderCount = 0;
    uint32_t width = 0;
    uint32_t height = 0;
    uint32_t numChannels = 0;
    DataType dataType = type_invalid;
    TextureType textureType = texture_invalid;
};

struct SBParameter
{
    enum Type {
        type_invalid,
        type_bool,
        type_action,
        type_int,
        type_double,
        type_ivec2,
        type_dvec2,
        type_dvec3,
        type_dvec4,
        type_dmat4,
        type_string,
        type_image,
        type_messages,
        type_audio,
    };

    Type type = type_invalid;
    const char *name = nullptr;
    union Value {
        bool boolValue = false;
        bool action;
        int64_t intValue;
        double doubleValue;
        int ivec[2];
        double dvec[4];
        double dmat4[16];
        SBImageInfo imageInfo;
        const char *string;
    };
    Value value = {};
    bool changed = false;
};

class SBMessenger
{
public:
    virtual ~SBMessenger() = default;

    virtual int getNextMessengerMessageSize() = 0;
    virtual bool popNextMessengerMessage(char *destBuffer, int maxSize) = 0;

    virtual void sendMessengerMessage(const char *msg, int size) = 0;
};

struct SBClientInfo
{
    char connectedServerIp[32] = {};
};

class SBPrivateAPI;

class SBHost : public SBMessenger
{
public:
    // Parameters
    virtual SBParameter *addInputParameter(SBParameter::Type type, const char *name,
                                           const SBParameter::Value &defaultValue = {})
        = 0;
    virtual SBParameter *getInputParameter(const char *name) = 0;

    virtual bool addOutputParameter(SBParameter::Type type, const char *name,
                                    const SBParameter::Value &defaultValue = {})
        = 0;
    virtual bool setOutputParameter(const char *name, const SBParameter::Value &value) = 0;

    virtual bool removeParameter(const char *name) = 0;

    // Input Parameter Messages
    virtual int getNextInputParameterMessageSize(const char *name) = 0;
    virtual bool popNextInputParameterMessage(const char *name, char *destBuffer, int maxSize) = 0;

    virtual bool pushOutputParameterMessageEvent(const char *name, const char *value) = 0;
    virtual bool pushOutputParameterMessageJson(const char *name, const char *json) = 0;

    // General
    virtual void disableWatchdogTimer(double seconds) = 0;

    virtual const char *projectName() const = 0;
    virtual const char *projectPath() const = 0;
    virtual const char *appDataPath() const = 0;
    virtual const char *binaryPath() const = 0;

    virtual double fps() const = 0;
    virtual int64_t currentFameNumber() const = 0;
    virtual bool isQuadroCard() const = 0;

    virtual SBPrivateAPI *getSBPrivateAPI() = 0;

    // Logging
    virtual void log(const char *str) = 0;
    virtual void logWarning(const char *str) = 0;
    virtual void logError(const char *str) = 0;

    // Audio, these can be called only from inside processAudio()
    virtual int getInputAudioChannels(const char *name) const = 0;
    virtual bool readInputAudio(const char *name, int channel, float *destBuffer,
                                int maxSamples) const
        = 0;

    virtual bool setOutputAudioChannels(const char *name, int channels) = 0;
    virtual bool writeOutputAudio(const char *name, int channel, float *data, int samples) = 0;

    virtual int getAudioSamplerate() const = 0;
};

class SBNode
{
public:
    virtual ~SBNode() = default;

    // process is called every frame
    virtual void process() {}

    // buffers swapped is called after swapBuffers() returns
    virtual void contextBuffersSwapped() {}

    // ProcessAudio will be called from a separate audio thread
    virtual void processAudio(int samples) {}
};

#endif // SBNODE_H
