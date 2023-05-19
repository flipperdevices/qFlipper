#pragma once

#include "mainresponse.h"
#include "storageresponseinterface.h"

class StorageInfoResponse : public MainResponse, public StorageInfoResponseInterface
{
    Q_OBJECT
    Q_INTERFACES(StorageInfoResponseInterface)

public:
    StorageInfoResponse(MessageWrapper &wrapper, QObject *parent = nullptr);
    quint64 sizeFree() const override;
    quint64 sizeTotal() const override;
};

class StorageStatResponse : public MainResponse, public StorageStatResponseInterface
{
    Q_OBJECT
    Q_INTERFACES(StorageStatResponseInterface)

public:
    StorageStatResponse(MessageWrapper &wrapper, QObject *parent = nullptr);
    bool hasFile() const override;
    const StorageFile file() const override;
};

class StorageListResponse : public MainResponse, public StorageListResponseInterface
{
    Q_OBJECT
    Q_INTERFACES(StorageListResponseInterface)

public:
    StorageListResponse(MessageWrapper &wrapper, QObject *parent = nullptr);
    const StorageFiles files() const override;
};

class StorageReadResponse : public MainResponse, public StorageReadResponseInterface
{
    Q_OBJECT
    Q_INTERFACES(StorageReadResponseInterface)

public:
    StorageReadResponse(MessageWrapper &wrapper, QObject *parent = nullptr);
    bool hasFile() const override;
    const StorageFile file() const override;
};

class StorageMd5SumResponse : public MainResponse, public StorageMd5SumResponseInterface
{
    Q_OBJECT
    Q_INTERFACES(StorageMd5SumResponseInterface)

public:
    StorageMd5SumResponse(MessageWrapper &wrapper, QObject *parent = nullptr);
    const QByteArray md5Sum() const override;
};
