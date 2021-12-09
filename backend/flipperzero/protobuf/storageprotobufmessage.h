#pragma once

#include "mainprotobufmessage.h"

namespace Flipper {
namespace Zero {

class StorageInfoRequest:
public AbstractMainProtobufRequest<PB_Main_storage_info_request_tag>
{
public:
    StorageInfoRequest(QSerialPort *serialPort, const QByteArray &path);

private:
    QByteArray m_path;
};

class StorageInfoResponse:
public AbstractMainProtobufResponse<PB_Main_storage_info_response_tag>
{
public:
    StorageInfoResponse(QSerialPort *serialPort);

    quint64 sizeFree() const;
    quint64 sizeTotal() const;
};

class StorageStatRequest:
public AbstractMainProtobufRequest<PB_Main_storage_stat_request_tag>
{
public:
    StorageStatRequest(QSerialPort *serialPort, const QByteArray &fileName);

private:
    QByteArray m_fileName;
};

class StorageStatResponse:
public AbstractMainProtobufResponse<PB_Main_storage_stat_response_tag>
{
public:
    StorageStatResponse(QSerialPort *serialPort);

    bool isPresent() const;
    const PB_Storage_File fileInfo() const;
};

}
}

