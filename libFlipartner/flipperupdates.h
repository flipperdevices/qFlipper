#ifndef FLIPPERUPDATES_H
#define FLIPPERUPDATES_H

#include <QObject>
#include <QVector>
#include <QString>
#include <QByteArray>
#include <QJsonValue>

namespace FlipperUpdates {

struct FileInfo
{
    FileInfo() = default;
    FileInfo(const QJsonValue &val);

    QString target;
    QString type;
    QString url;
    QByteArray sha512;
};

struct VersionInfo
{
    VersionInfo() = default;
    VersionInfo(const QJsonValue &val);

    QString version;
    QString changelog;
    time_t timestamp;
    QVector<FileInfo> files;
};

struct ChannelInfo
{
    ChannelInfo() = default;
    ChannelInfo(const QJsonValue &val);

    QString id;
    QString title;
    QString description;
    QVector<VersionInfo> versions;
};

}

Q_DECLARE_METATYPE(FlipperUpdates::FileInfo)
Q_DECLARE_METATYPE(FlipperUpdates::VersionInfo)
Q_DECLARE_METATYPE(FlipperUpdates::ChannelInfo)

#endif // FLIPPERUPDATES_H
