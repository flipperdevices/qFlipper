#include "storageprotobufmessage.h"

using namespace Flipper;
using namespace Zero;

StorageInfoRequest::StorageInfoRequest(QSerialPort *serialPort, const QByteArray &path):
    AbstractMainProtobufRequest(serialPort),
    m_path(path)
{
    pbMessage()->content.storage_info_request.path = m_path.data();
}

StorageInfoResponse::StorageInfoResponse(QSerialPort *serialPort):
    AbstractMainProtobufResponse(serialPort)
{}

quint64 StorageInfoResponse::sizeFree() const
{
    return pbMessage()->content.storage_info_response.free_space;
}

quint64 StorageInfoResponse::sizeTotal() const
{
    return pbMessage()->content.storage_info_response.total_space;
}
