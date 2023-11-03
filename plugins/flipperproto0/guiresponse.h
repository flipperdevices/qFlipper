#pragma once

#include "mainresponse.h"
#include "guiresponseinterface.h"

class GuiScreenFrameResponse : public MainResponse, public GuiScreenFrameResponseInterface
{
    Q_OBJECT
    Q_INTERFACES(GuiScreenFrameResponseInterface)

public:
    GuiScreenFrameResponse(MessageWrapper &wrapper, QObject *parent = nullptr);
    const QByteArray screenFrame() const override;
    Qt::ScreenOrientation screenOrientation() const override;
};
