#include "protobufplugin.h"

#include "messages/flipper.pb.h"

class ConcreteProtobufMessage : public AbstractProtobufMessage
{
public:
    ConcreteProtobufMessage():
        m_message{69, PB_CommandStatus_OK, false, {}, 0, {}}
    {}

    uint32_t commandID() const override
    {
        return m_message.command_id;
    }

    bool hasNext() const override
    {
        return m_message.has_next;
    }

private:
    PB_Main m_message;
};

ProtobufPlugin::ProtobufPlugin(QObject *parent):
    QObject(parent)
{}

const QByteArray ProtobufPlugin::systemDeviceInfo(uint32_t commandID) const
{
    return QByteArray::number(commandID, 16);
}

AbstractProtobufMessage *ProtobufPlugin::decode(const QByteArray &buffer) const
{
    Q_UNUSED(buffer)
    return new ConcreteProtobufMessage;
}
