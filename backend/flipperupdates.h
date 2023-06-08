#pragma once

#include <QObject>
#include <QVector>
#include <QString>
#include <QVariant>
#include <QByteArray>
#include <QJsonValue>

namespace Flipper {
namespace Updates {

class FileInfo
{
    Q_GADGET

public:
    FileInfo();
    FileInfo(const QJsonValue &val);

    const QString &target() const;
    const QString &type() const;
    const QString &url() const;
    const QByteArray &sha256() const;

    bool isValid() const;

private:
    QString m_target;
    QString m_type;
    QString m_url;
    QByteArray m_sha256;
    bool m_isValid;
};

class VersionInfo
{
    Q_GADGET
    Q_PROPERTY(QString number READ number CONSTANT)
    Q_PROPERTY(QString changelog READ changelog CONSTANT)
    Q_PROPERTY(QDate date READ date CONSTANT)
    Q_PROPERTY(QVector<Flipper::Updates::FileInfo> files READ files CONSTANT)

public:
    VersionInfo() = default;
    VersionInfo(const QJsonValue &val);

    const QString &number() const;
    const QString &changelog() const;
    const QDate &date() const;
    const QVector<Flipper::Updates::FileInfo> &files() const;

    const Flipper::Updates::FileInfo fileInfo(const QString &type, const QString &target) const;
    qint64 compare(const VersionInfo &other) const;

    static qint64 toNumericValue(const QString &version);
    static qint64 compare(const QString &v1, const QString &v2);

private:
    QString m_number;
    QString m_changelog;
    QDate m_date;
    QVector<FileInfo> m_files;
};

class ChannelInfo
{
    Q_GADGET
    Q_PROPERTY(QString name READ name CONSTANT)
    Q_PROPERTY(QString title READ title CONSTANT)
    Q_PROPERTY(QString description READ description CONSTANT)
    Q_PROPERTY(QVector<Flipper::Updates::VersionInfo> versions READ versions CONSTANT)
    Q_PROPERTY(Flipper::Updates::VersionInfo latestVersion READ latestVersion CONSTANT)

public:
    ChannelInfo() = default;
    ChannelInfo(const QJsonValue &val);

    const QString &name() const;
    const QString &title() const;
    const QString &description() const;
    const QVector<Flipper::Updates::VersionInfo> &versions() const;
    const Flipper::Updates::VersionInfo &latestVersion() const;

private:
    QString m_id;
    QString m_title;
    QString m_description;
    QVector<VersionInfo> m_versions;
};

}
}

Q_DECLARE_METATYPE(Flipper::Updates::FileInfo)
Q_DECLARE_METATYPE(Flipper::Updates::VersionInfo)
Q_DECLARE_METATYPE(Flipper::Updates::ChannelInfo)
