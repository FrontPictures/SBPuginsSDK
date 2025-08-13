#ifndef TESTPLUGIN_H
#define TESTPLUGIN_H

#include <QWidget>
#include <QLabel>

#ifdef _WIN32
#define PLUGIN_EXPORTS __declspec(dllexport)
#else
#define PLUGIN_EXPORTS __attribute__((visibility("default")))
#endif

class SBMessanger;
struct SBClientInfo;

extern "C" PLUGIN_EXPORTS QWidget *createSBPluginWidget(SBMessanger *messanger,
                                                        const SBClientInfo &info);

class TestPluginWidget : public QWidget
{
    Q_OBJECT
public:
    TestPluginWidget(SBMessanger *messanger, const SBClientInfo &info);

private:
    void processMessages();

    SBMessanger *mMessanger = nullptr;
    QLabel *mStateLabel = nullptr;
};

#endif // TESTPLUGIN_H
