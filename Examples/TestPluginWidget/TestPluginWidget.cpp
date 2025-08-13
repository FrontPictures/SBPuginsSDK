#include "TestPluginWidget.h"
#include "SBNode.h"

#include <QVBoxLayout>
#include <QTimer>
#include <QPushButton>

QWidget *createSBPluginWidget(SBMessanger *messanger, const SBClientInfo &info)
{
    return new TestPluginWidget(messanger, info);
}

TestPluginWidget::TestPluginWidget(SBMessanger *messanger, const SBClientInfo &info)
    : mMessanger(messanger)
{
    printf("Connected server IP address: %s\n", info.connectedServerIp);

    auto *layout = new QVBoxLayout();
    layout->addStretch();
    layout->addWidget(new QLabel("Test Plugin Widget"));

    if (!mMessanger) {
        printf("Error: No messanger\n");
        return;
    }

    mMessanger->sendMessage("GetState");

    auto *button = new QPushButton("Increment");
    connect(button, &QPushButton::clicked, this, [this](){
        mMessanger->sendMessage("Increment");
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
    while (auto msg = mMessanger->nextMessage()) {
        mStateLabel->setText(QString("Current value: %1")
                                 .arg(QString::fromStdString(std::string(msg.ptr, msg.size))));
    }
}
