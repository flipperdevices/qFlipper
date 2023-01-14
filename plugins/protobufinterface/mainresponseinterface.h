#pragma once

#include <QString>
#include <QtPlugin>

class MainResponseInterface
{
public:
    enum ResponseType {
        Unknown,
        Empty,

        StatusPing,
        SystemDeviceInfo,
        SystemGetDateTime,
        SystemUpdate,
        SystemProtobufVersion,

        StorageList,
        StorageRead,
        StorageMd5Sum,
        StorageStat,
        StorageInfo,

        GuiScreenFrame,

        PropertyGet,
    };

    virtual ~MainResponseInterface() {}

    virtual uint32_t id() const = 0;
    virtual ResponseType type() const = 0;
    virtual size_t encodedSize() const = 0;

    virtual bool hasNext() const = 0;
    virtual bool isError() const = 0;

    virtual const QString errorString() const = 0;
};

using EmptyResponseInterface = MainResponseInterface;

QT_BEGIN_NAMESPACE
Q_DECLARE_INTERFACE(MainResponseInterface, "com.flipperdevices.MainResponseInterface/1.0")
QT_END_NAMESPACE
