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

StorageStatRequest::StorageStatRequest(QSerialPort *serialPort, const QByteArray &path):
    AbstractMainProtobufRequest(serialPort),
    m_path(path)
{
   pbMessage()->content.storage_stat_request.path = m_path.data();
}

StorageStatResponse::StorageStatResponse(QSerialPort *serialPort):
    AbstractMainProtobufResponse(serialPort)
{}

bool StorageStatResponse::isPresent() const
{
    return pbMessage()->content.storage_stat_response.has_file;
}

const PB_Storage_File StorageStatResponse::file() const
{
    return pbMessage()->content.storage_stat_response.file;
}

StorageListRequest::StorageListRequest(QSerialPort *serialPort, const QByteArray &path):
    AbstractMainProtobufRequest(serialPort),
    m_path(path)
{
    pbMessage()->content.storage_list_request.path = m_path.data();
}

StorageListResponse::StorageListResponse(QSerialPort *serialPort):
    AbstractMainProtobufResponse(serialPort)
{}

QVector<PB_Storage_File> StorageListResponse::files() const
{
    const auto count = pbMessage()->content.storage_list_response.file_count;

    QVector<PB_Storage_File> ret;
    ret.reserve(count);

    for(auto i = 0; i < count ; ++i) {
        ret.append(pbMessage()->content.storage_list_response.file[i]);
    }

    return ret;
}

StorageMkdirRequest::StorageMkdirRequest(QSerialPort *serialPort, const QByteArray &path):
    AbstractMainProtobufRequest(serialPort),
    m_path(path)
{
    pbMessage()->content.storage_mkdir_request.path = m_path.data();
}

StorageRemoveRequest::StorageRemoveRequest(QSerialPort *serialPort, const QByteArray &path, bool recursive):
    AbstractMainProtobufRequest(serialPort),
    m_path(path)
{
    pbMessage()->content.storage_delete_request.path = m_path.data();
    pbMessage()->content.storage_delete_request.recursive = recursive;
}

StorageReadRequest::StorageReadRequest(QSerialPort *serialPort, const QByteArray &path):
    AbstractMainProtobufRequest(serialPort),
    m_path(path)
{
    pbMessage()->content.storage_read_request.path = m_path.data();
}

StorageReadResponse::StorageReadResponse(QSerialPort *serialPort):
    AbstractMainProtobufResponse(serialPort)
{}

bool StorageReadResponse::isPresent() const
{
    return pbMessage()->content.storage_read_response.has_file;
}

const QByteArray StorageReadResponse::data() const
{
    const auto &file = pbMessage()->content.storage_read_response.file;
    return QByteArray((const char*)file.data, file.size);
}

StorageWriteRequest::StorageWriteRequest(QSerialPort *serialPort, const QByteArray &path, const QByteArray &buf):
    AbstractMainProtobufRequest(serialPort),
    m_path(path)
{
    auto &request = pbMessage()->content.storage_write_request;
    request.path = m_path.data();
    request.file.data = (pb_bytes_array_t*)malloc(PB_BYTES_ARRAY_T_ALLOCSIZE(buf.size()));
    request.file.data->size = buf.size();
    memcpy(request.file.data->bytes, buf.data(), buf.size());
}

StorageWriteRequest::~StorageWriteRequest()
{
    auto &request = pbMessage()->content.storage_write_request;
    free(request.file.data);
}
