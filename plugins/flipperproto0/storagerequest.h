#pragma once

#include "mainrequest.h"

class AbstractStorageRequest : public MainRequest
{
protected:
    AbstractStorageRequest(uint32_t id, pb_size_t tag, const QByteArray &path, bool hasNext = false);
    char *pathData();

private:
    QByteArray m_path;
};

class StorageInfoRequest : public AbstractStorageRequest
{
public:
    StorageInfoRequest(uint32_t id, const QByteArray &path);
};

class StorageStatRequest : public AbstractStorageRequest
{
public:
    StorageStatRequest(uint32_t id, const QByteArray &path);
};

class StorageListRequest : public AbstractStorageRequest
{
public:
    StorageListRequest(uint32_t id, const QByteArray &path);
};

class StorageMkDirRequest : public AbstractStorageRequest
{
public:
    StorageMkDirRequest(uint32_t id, const QByteArray &path);
};

class StorageRenameRequest : public AbstractStorageRequest
{
public:
    StorageRenameRequest(uint32_t id, const QByteArray &oldPath, const QByteArray &newPath);
private:
    QByteArray m_newPath;
};

class StorageRemoveRequest : public AbstractStorageRequest
{
public:
    StorageRemoveRequest(uint32_t id, const QByteArray &path, bool recursive);
};

class StorageReadRequest : public AbstractStorageRequest
{
public:
    StorageReadRequest(uint32_t id, const QByteArray &path);
};

class StorageWriteRequest : public AbstractStorageRequest
{
public:
    StorageWriteRequest(uint32_t id, const QByteArray &path, const QByteArray &data, bool hasNext);
    ~StorageWriteRequest();
};

class StorageMd5SumRequest : public AbstractStorageRequest
{
public:
    StorageMd5SumRequest(uint32_t id, const QByteArray &path);
};
