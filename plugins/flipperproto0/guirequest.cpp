#include "guirequest.h"

#include "pb_encode.h"

GuiStartScreenStreamRequest::GuiStartScreenStreamRequest(uint32_t id):
    MainRequest(id, PB_Main_gui_start_screen_stream_request_tag)
{}

GuiStopScreenStreamRequest::GuiStopScreenStreamRequest(uint32_t id):
    MainRequest(id, PB_Main_gui_stop_screen_stream_request_tag)
{}

GuiScreenFrameRequest::GuiScreenFrameRequest(uint32_t id, const QByteArray &screenData):
    MainRequest(id, PB_Main_gui_screen_frame_tag)
{
    auto &content = m_message.content.gui_screen_frame;
    content.data = (pb_bytes_array_t*)malloc(PB_BYTES_ARRAY_T_ALLOCSIZE(screenData.size()));
    content.data->size = screenData.size();
    memcpy(content.data->bytes, screenData.data(), screenData.size());
}

GuiScreenFrameRequest::~GuiScreenFrameRequest()
{
    auto &content = m_message.content.gui_screen_frame;
    free(content.data);
}

GuiSendInputRequest::GuiSendInputRequest(uint32_t id, PB_Gui_InputKey key, PB_Gui_InputType type):
    MainRequest(id, PB_Main_gui_send_input_event_request_tag)
{
    auto &content = m_message.content.gui_send_input_event_request;
    content.key = key;
    content.type = type;
}

GuiStartVirtualDisplayRequest::GuiStartVirtualDisplayRequest(uint32_t id, const QByteArray &screenData):
    MainRequest(id, PB_Main_gui_start_virtual_display_request_tag)
{
    if(screenData.isEmpty()) {
        return;
    }

    auto &content = m_message.content.gui_start_virtual_display_request;
    content.first_frame.data = (pb_bytes_array_t*)malloc(PB_BYTES_ARRAY_T_ALLOCSIZE(screenData.size()));
    content.first_frame.data->size = screenData.size();
    content.has_first_frame = true;
    memcpy(content.first_frame.data->bytes, screenData.data(), screenData.size());
}

GuiStartVirtualDisplayRequest::~GuiStartVirtualDisplayRequest()
{
    auto &content = m_message.content.gui_start_virtual_display_request;
    if(content.has_first_frame) {
        free(content.first_frame.data);
    }
}

GuiStopVirtualDisplayRequest::GuiStopVirtualDisplayRequest(uint32_t id):
    MainRequest(id, PB_Main_gui_stop_virtual_display_request_tag)
{}
