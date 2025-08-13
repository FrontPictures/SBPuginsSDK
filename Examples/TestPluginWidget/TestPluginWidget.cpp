#include "TestPluginWidget.h"
#include "SBNode.h"

#include <QVBoxLayout>
#include <QTimer>
#include <QPushButton>

QWidget *createSBPluginWidget2(SBMessenger *messenger, const SBClientInfo &info)
{
    return new TestPluginWidget(messenger, info);
}

TestPluginWidget::TestPluginWidget(SBMessenger *messenger, const SBClientInfo &info)
    : mMessenger(messenger)
{
    printf("Connected server IP address: %s\n", info.connectedServerIp);

    auto *layout = new QVBoxLayout();
    layout->addStretch();
    layout->addWidget(new QLabel("Test Plugin Widget"));

    if (!mMessenger) {
        printf("Error: No messenger\n");
        return;
    }

    std::string getStateMsg = "GetState";
    mMessenger->sendMessengerMessage(getStateMsg.c_str(), getStateMsg.size());

    auto *button = new QPushButton("Increment");
    connect(button, &QPushButton::clicked, this, [this]() {
        std::string incrementMsg = "Increment";
        mMessenger->sendMessengerMessage(incrementMsg.c_str(), incrementMsg.size());
    });
    layout->addWidget(button);

    mStateLabel = new QLabel("Current value: unknown");
    layout->addWidget(mStateLabel);

    layout->addStretch();

    auto *hLayout = new QHBoxLayout(this);
    hLayout->addStretch();
    hLayout->addLayout(layout);
    hLayout->addStretch();

    auto *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &TestPluginWidget::processMessages);
    timer->start(30);
}

void TestPluginWidget::processMessages()
{
    while (int size = mMessenger->getNextMessengerMessageSize()) {
        std::string msg;
        msg.resize(size);
        mMessenger->popNextMessengerMessage(msg.data(), msg.size());
        mStateLabel->setText(QString("Current value: %1").arg(QString::fromStdString(msg)));
    }
}
