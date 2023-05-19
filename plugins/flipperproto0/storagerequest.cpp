#include "storagerequest.h"

#include "pb_encode.h"

AbstractStorageRequest::AbstractStorageRequest(uint32_t id, pb_size_t tag, const QByteArray &path, bool hasNext):
    MainRequest(id, tag, hasNext),
    m_path(path)
{}

char *AbstractStorageRequest::pathData()
{
    return m_path.data();
}

StorageInfoRequest::StorageInfoRequest(uint32_t id, const QByteArray &path):
    AbstractStorageRequest(id, PB_Main_storage_info_request_tag, path)
{
    m_message.content.storage_info_request.path = pathData();
}

StorageStatRequest::StorageStatRequest(uint32_t id, const QByteArray &path):
    AbstractStorageRequest(id, PB_Main_storage_stat_request_tag, path)
{
    m_message.content.storage_stat_request.path = pathData();
}

StorageListRequest::StorageListRequest(uint32_t id, const QByteArray &path):
    AbstractStorageRequest(id, PB_Main_storage_list_request_tag, path)
{
    m_message.content.storage_list_request.path = pathData();
}

StorageMkDirRequest::StorageMkDirRequest(uint32_t id, const QByteArray &path):
    AbstractStorageRequest(id, PB_Main_storage_mkdir_request_tag, path)
{
    m_message.content.storage_mkdir_request.path = pathData();
}

StorageRemoveRequest::StorageRemoveRequest(uint32_t id, const QByteArray &path, bool recursive):
    AbstractStorageRequest(id, PB_Main_storage_delete_request_tag, path)
{
    m_message.content.storage_delete_request.path = pathData();
    m_message.content.storage_delete_request.recursive = recursive;
}

StorageReadRequest::StorageReadRequest(uint32_t id, const QByteArray &path):
    AbstractStorageRequest(id, PB_Main_storage_read_request_tag, path)
{
    m_message.content.storage_read_request.path = pathData();
}

StorageWriteRequest::StorageWriteRequest(uint32_t id, const QByteArray &path, const QByteArray &data, bool hasNext):
    AbstractStorageRequest(id, PB_Main_storage_write_request_tag, path, hasNext)
{
    auto &content = m_message.content.storage_write_request;

    content.has_file = !data.isEmpty();
    content.path = pathData();

    if(content.has_file) {
        content.file.data = (pb_bytes_array_t*)malloc(PB_BYTES_ARRAY_T_ALLOCSIZE(data.size()));
        content.file.data->size = data.size();
        memcpy(content.file.data->bytes, data.data(), data.size());
    }
}

StorageWriteRequest::~StorageWriteRequest()
{
    auto &content = m_message.content.storage_write_request;

    if(content.has_file) {
        free(content.file.data);
    }
}

StorageRenameRequest::StorageRenameRequest(uint32_t id, const QByteArray &oldPath, const QByteArray &newPath):
    AbstractStorageRequest(id, PB_Main_storage_rename_request_tag, oldPath),
    m_newPath(newPath)
{
    auto &request = m_message.content.storage_rename_request;
    request.old_path = pathData();
    request.new_path = (char*)newPath.data();
}

StorageMd5SumRequest::StorageMd5SumRequest(uint32_t id, const QByteArray &path):
    AbstractStorageRequest(id, PB_Main_storage_md5sum_request_tag, path)
{
    m_message.content.storage_md5sum_request.path = pathData();
}
