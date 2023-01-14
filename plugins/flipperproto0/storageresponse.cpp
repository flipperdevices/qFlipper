#include "storageresponse.h"

StorageInfoResponse::StorageInfoResponse(MessageWrapper &wrapper, QObject *parent):
    MainResponse(wrapper, parent)
{}

quint64 StorageInfoResponse::sizeFree() const
{
    return message().content.storage_info_response.free_space;
}

quint64 StorageInfoResponse::sizeTotal() const
{
    return message().content.storage_info_response.total_space;
}

StorageStatResponse::StorageStatResponse(MessageWrapper &wrapper, QObject *parent):
    MainResponse(wrapper, parent)
{}

bool StorageStatResponse::hasFile() const
{
    return message().content.storage_stat_response.has_file;
}

const StorageFile StorageStatResponse::file() const
{
    const auto &f = message().content.storage_stat_response.file;
    return {(StorageFile::FileType)f.type, {f.name}, {}, f.size};
}

StorageListResponse::StorageListResponse(MessageWrapper &wrapper, QObject *parent):
    MainResponse(wrapper, parent)
{}

const StorageListResponse::StorageFiles StorageListResponse::files() const
{
    auto count = message().content.storage_list_response.file_count;
    auto *fs = message().content.storage_list_response.file;

    StorageFiles ret;
    ret.reserve(count);

    for(; count; --count, ++fs) {
        ret.append({(StorageFile::FileType)fs->type, {fs->name}, {}, fs->size});
    }

    return ret;
}

StorageReadResponse::StorageReadResponse(MessageWrapper &wrapper, QObject *parent):
    MainResponse(wrapper, parent)
{}

bool StorageReadResponse::hasFile() const
{
    return message().content.storage_read_response.has_file;
}

const StorageFile StorageReadResponse::file() const
{
    const auto &f = message().content.storage_read_response.file;
    return {(StorageFile::FileType)f.type, {f.name}, {(const char*)f.data->bytes, f.data->size}, f.size};
}

StorageMd5SumResponse::StorageMd5SumResponse(MessageWrapper &wrapper, QObject *parent):
    MainResponse(wrapper, parent)
{}

const QByteArray StorageMd5SumResponse::md5Sum() const
{
    return message().content.storage_md5sum_response.md5sum;
}
