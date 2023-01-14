#pragma once

#include <QVector>
#include <QtPlugin>
#include <QByteArray>

struct StorageFile
{
    enum FileType {
        RegularFile = 0,
        Directory = 1
    };

    FileType type;
    QByteArray name;
    QByteArray data;
    quint64 size;
};

class StorageInfoResponseInterface
{
public:
    virtual quint64 sizeFree() const = 0;
    virtual quint64 sizeTotal() const = 0;
};

class StorageStatResponseInterface
{
public:
    virtual bool hasFile() const = 0;
    virtual const StorageFile file() const = 0;
};

class StorageListResponseInterface
{
public:
    using StorageFiles = QVector<StorageFile>;
    virtual const StorageFiles files() const = 0;
};

class StorageReadResponseInterface
{
public:
    virtual bool hasFile() const = 0;
    virtual const StorageFile file() const = 0;
};

class StorageMd5SumResponseInterface
{
public:
    virtual const QByteArray md5Sum() const = 0;
};

QT_BEGIN_NAMESPACE
Q_DECLARE_INTERFACE(StorageInfoResponseInterface, "com.flipperdevices.StorageInfoResponseInterface/1.0")
Q_DECLARE_INTERFACE(StorageStatResponseInterface, "com.flipperdevices.StorageStatResponseInterface/1.0")
Q_DECLARE_INTERFACE(StorageListResponseInterface, "com.flipperdevices.StorageListResponseInterface/1.0")
Q_DECLARE_INTERFACE(StorageReadResponseInterface, "com.flipperdevices.StorageReadResponseInterface/1.0")
Q_DECLARE_INTERFACE(StorageMd5SumResponseInterface, "com.flipperdevices.StorageMd5SumResponseInterface/1.0")
QT_END_NAMESPACE
