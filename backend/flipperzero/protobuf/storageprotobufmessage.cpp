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

StorageStatRequest::StorageStatRequest(QSerialPort *serialPort, const QByteArray &fileName):
    AbstractMainProtobufRequest(serialPort),
    m_fileName(fileName)
{
   pbMessage()->content.storage_stat_request.path = m_fileName.data();
}

StorageStatResponse::StorageStatResponse(QSerialPort *serialPort):
    AbstractMainProtobufResponse(serialPort)
{}

bool StorageStatResponse::isPresent() const
{
    return pbMessage()->content.storage_stat_response.has_file;
}

const PB_Storage_File StorageStatResponse::fileInfo() const
{
    return pbMessage()->content.storage_stat_response.file;
}
