#ifndef TESTPLUGIN_H
#define TESTPLUGIN_H

#include <QWidget>
#include <QLabel>

#ifdef _WIN32
#define PLUGIN_EXPORTS __declspec(dllexport)
#else
#define PLUGIN_EXPORTS __attribute__((visibility("default")))
#endif

class SBMessenger;
struct SBClientInfo;

extern "C" PLUGIN_EXPORTS QWidget *createSBPluginWidget2(SBMessenger *messenger,
                                                         const SBClientInfo &info);

class TestPluginWidget : public QWidget
{
    Q_OBJECT
public:
    TestPluginWidget(SBMessenger *messenger, const SBClientInfo &info);

private:
    void processMessages();

    SBMessenger *mMessenger = nullptr;
    QLabel *mStateLabel = nullptr;
};

#endif // TESTPLUGIN_H
