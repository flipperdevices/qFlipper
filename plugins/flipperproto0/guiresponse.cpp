#include "guiresponse.h"

GuiScreenFrameResponse::GuiScreenFrameResponse(MessageWrapper &wrapper, QObject *parent):
    MainResponse(wrapper, parent)
{}

const QByteArray GuiScreenFrameResponse::screenFrame() const
{
    const auto *f = message().content.gui_screen_frame.data;
    return QByteArray((const char*)f->bytes, f->size);
}

GuiScreenFrameResponseInterface::ScreenOrientation GuiScreenFrameResponse::screenOrientation() const
{
    switch(message().content.gui_screen_frame.orientation) {
    case PB_Gui_ScreenOrientation_HORIZONTAL_FLIP:
        return ScreenOrientation::HorizontalFlip;
    case PB_Gui_ScreenOrientation_VERTICAL:
        return ScreenOrientation::Vertical;
    case PB_Gui_ScreenOrientation_VERTICAL_FLIP:
        return ScreenOrientation::VerticalFlip;
    case PB_Gui_ScreenOrientation_HORIZONTAL:
    default:
        return ScreenOrientation::Horizontal;
    }
}
