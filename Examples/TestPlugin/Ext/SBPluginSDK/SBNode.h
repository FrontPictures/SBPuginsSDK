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
extern "C" PLUGIN_EXPORTS SBNode *createSBPluginNode();

// for panel widget plugin:
extern "C" PLUGIN_EXPORTS QWidget *createSBPluginWidget(SBMessanger *messanger,
                                                        const SBClientInfo &info);

*/

#include <cstdint>
#include <cstddef>
#include <cstring>

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
        type_image
    };

    Type type = type_invalid;
    const char *name = nullptr;
    union Value {
        bool boolValue;
        bool action;
        int64_t intValue;
        double doubleValue;
        int ivec[2];
        double dvec[4];
        double dmat4[16];
        SBImageInfo imageInfo;
        const char *string; // string must be owned by plugin
    };
    Value value = {};
};

struct SBParameters
{
    SBParameter *parameters = nullptr;
    size_t count = 0;
};

class SBPrivateAPI;

class SBHost
{
public:
    virtual ~SBHost() = default;

    virtual void disableWatchdogTimer(double seconds) = 0;

    virtual const char *projectName() const = 0;
    virtual const char *projectPath() const = 0;
    virtual const char *appDataPath() const = 0;
    virtual const char *binaryPath() const = 0;

    virtual double fps() const = 0;
    virtual int64_t currentFameNumber() const = 0;
    virtual bool isQuadroCard() const = 0;

    virtual SBPrivateAPI *getSBPrivateAPI() = 0;
};

struct SBMessage
{
    SBMessage() = default;
    SBMessage(const char *ptr, size_t size) : ptr(ptr), size(size) {}
    SBMessage(const char *ptr) : ptr(ptr), size(std::strlen(ptr)) {}

    const char *ptr = nullptr;
    size_t size = 0;

    explicit operator bool() const { return ptr != nullptr && size > 0; }
};

class SBMessanger
{
public:
    virtual ~SBMessanger() = default;

    virtual SBMessage nextMessage() = 0;
    virtual void sendMessage(SBMessage msg) = 0;
};

struct SBClientInfo
{
    char connectedServerIp[32] = {};
};

class SBNode
{
public:
    virtual ~SBNode() = default;

    // These will be called once after SBNode instance created
    virtual void setSBHost(SBHost *host) {}
    virtual void setSBMessanger(SBMessanger *messanger) {}

    // input/output info methods will be called only once per node instance
    // it won't be called during plugin node reinitialization, while SBNode instance recreates
    virtual size_t getNumberOfInputs() const { return 0; }
    virtual void fillInputInfo(SBParameters inputs) const {}

    virtual size_t getNumberOfOutputs() const { return 0; }
    virtual void fillOutputInfo(SBParameters outputs) const {}

    // process is called every frame
    virtual void process(SBParameters inputs, SBParameters outputs) {}

    // buffers swapped is called after swapBuffers() returns
    virtual void contextBuffersSwapped() {}
};

#endif // SBNODE_H
