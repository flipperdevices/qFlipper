#include "guiresponse.h"

GuiScreenFrameResponse::GuiScreenFrameResponse(MessageWrapper &wrapper, QObject *parent):
    MainResponse(wrapper, parent)
{}

const QByteArray GuiScreenFrameResponse::screenFrame() const
{
    const auto *f = message().content.gui_screen_frame.data;
    return QByteArray((const char*)f->bytes, f->size);
}
