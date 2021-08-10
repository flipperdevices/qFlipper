#ifndef FLIPPERUPDATES_H
#define FLIPPERUPDATES_H

#include <QObject>
#include <QVector>
#include <QString>
#include <QVariant>
#include <QByteArray>
#include <QJsonValue>

namespace Flipper {
namespace Updates {

struct FileInfo
{
    Q_GADGET

public:
    FileInfo() = default;
    FileInfo(const QJsonValue &val);

    QString target;
    QString type;
    QString url;
    QByteArray sha512;
};

struct VersionInfo
{
    Q_GADGET
    Q_PROPERTY(QString number MEMBER version)
    Q_PROPERTY(QString changelog MEMBER changelog)

public:
    VersionInfo() = default;
    VersionInfo(const QJsonValue &val);

    Q_INVOKABLE Flipper::Updates::FileInfo fileInfo(const QString &type, const QString &target) const;

    QString version;
    QString changelog;
    time_t timestamp;
    QVector<FileInfo> files;
};

struct ChannelInfo
{
    Q_GADGET

public:
    ChannelInfo() = default;
    ChannelInfo(const QJsonValue &val);

    Q_INVOKABLE Flipper::Updates::VersionInfo versionInfo(const QString &versionName) const;

    QString id;
    QString title;
    QString description;
    QVector<VersionInfo> versions;
};

}
}

Q_DECLARE_METATYPE(Flipper::Updates::FileInfo)
Q_DECLARE_METATYPE(Flipper::Updates::VersionInfo)
Q_DECLARE_METATYPE(Flipper::Updates::ChannelInfo)

#endif // FLIPPERUPDATES_H
