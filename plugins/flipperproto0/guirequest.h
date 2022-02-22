#pragma once

#include "mainrequest.h"

class GuiStartScreenStreamRequest: public MainRequest
{
public:
    GuiStartScreenStreamRequest(uint32_t id);
};

class GuiStopScreenStreamRequest : public MainRequest
{
public:
    GuiStopScreenStreamRequest(uint32_t id);
};

class GuiScreenFrameRequest : public MainRequest
{
public:
    GuiScreenFrameRequest(uint32_t id, const QByteArray &screenData);
    ~GuiScreenFrameRequest();
};

class GuiSendInputRequest : public MainRequest
{
public:
    GuiSendInputRequest(uint32_t id, PB_Gui_InputKey key, PB_Gui_InputType type);
};

class GuiStartVirtualDisplayRequest : public MainRequest
{
public:
    GuiStartVirtualDisplayRequest(uint32_t id, const QByteArray &screenData = QByteArray());
    ~GuiStartVirtualDisplayRequest();
};

class GuiStopVirtualDisplayRequest : public MainRequest
{
public:
    GuiStopVirtualDisplayRequest(uint32_t id);
};
