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
    StorageStatRequest(QSerialPort *serialPort, const QByteArray &path);

private:
    QByteArray m_path;
};

class StorageStatResponse:
public AbstractMainProtobufResponse<PB_Main_storage_stat_response_tag>
{
public:
    StorageStatResponse(QSerialPort *serialPort);

    bool isPresent() const;
    const PB_Storage_File file() const;
};

class StorageListRequest:
public AbstractMainProtobufRequest<PB_Main_storage_list_request_tag>
{
public:
    StorageListRequest(QSerialPort *serialPort, const QByteArray &path);

private:
    QByteArray m_path;
};

class StorageListResponse:
public AbstractMainProtobufResponse<PB_Main_storage_list_response_tag>
{
public:
    StorageListResponse(QSerialPort *serialPort);
    QVector<PB_Storage_File> files() const;
};

class StorageMkdirRequest:
public AbstractMainProtobufRequest<PB_Main_storage_mkdir_request_tag>
{
public:
    StorageMkdirRequest(QSerialPort *serialPort, const QByteArray &path);

private:
    QByteArray m_path;
};

class StorageRemoveRequest:
public AbstractMainProtobufRequest<PB_Main_storage_delete_request_tag>
{
public:
    StorageRemoveRequest(QSerialPort *serialPort, const QByteArray &path, bool recursive);

private:
    QByteArray m_path;
};

class StorageReadRequest:
public AbstractMainProtobufRequest<PB_Main_storage_read_request_tag>
{
public:
    StorageReadRequest(QSerialPort *serialPort, const QByteArray &path);

private:
    QByteArray m_path;
};

class StorageReadResponse:
public AbstractMainProtobufResponse<PB_Main_storage_read_response_tag>
{
public:
    StorageReadResponse(QSerialPort *serialPort);

    bool isPresent() const;
    const QByteArray data() const;
};

class StorageWriteRequest:
public AbstractMainProtobufRequest<PB_Main_storage_write_request_tag>
{
public:
    StorageWriteRequest(QSerialPort *serialPort, const QByteArray &path, const QByteArray &buf);
    ~StorageWriteRequest();

private:
    QByteArray m_path;
};

}
}

