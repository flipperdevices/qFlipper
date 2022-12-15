#pragma once

#include <QMap>
#include <QVector>
#include <QByteArray>
#include <QJsonValue>

#include "failable.h"

namespace Flipper {
namespace Zero {

class RadioManifest : public Failable
{
public:
    class Header {
    public:
        Header();
        Header(const QJsonValue &json);

        int version() const;
        qint64 timestamp() const;

    private:
        int m_version;
        qint64 m_timestamp;
    };

    class Condition {
    public:
        enum class Type {
            Unknown,
            Equals,
            Greater
        };

    Condition();
    Condition(const QString &text);

    Type type() const;
    const QString &version() const;

    private:
        Type m_type;
        QString m_version;
    };

    class FileInfo {
    public:
        FileInfo() = default;
        FileInfo(const QJsonValue &json);

        const QString &name() const;
        const QByteArray &sha256() const;
        const Condition &condition() const;
        uint32_t address() const;

    private:
        QString m_name;
        QByteArray m_sha256;
        Condition m_condition;
        uint32_t m_address;
    };

    using FileInfoMap = QMap<QString, FileInfo>;

    class Section {
    public:
        Section() = default;
        Section(const QJsonValue &json);

        const QString &version() const;
        const FileInfoMap &files() const;

    private:
        void readVersion(const QJsonValue &json);
        void readFiles(const QJsonValue &json);

        QString m_version;
        FileInfoMap m_files;
    };

    class RadioSection : public Section {
    public:
        RadioSection() = default;
        RadioSection(const QJsonValue &json);

        int type() const;
        int branch() const;
        int release() const;

    private:
        void readVersion(const QJsonValue &json);

        int m_type;
        int m_branch;
        int m_release;
    };

    class FirmwareInfo {
    public:
        FirmwareInfo() = default;
        FirmwareInfo(const QJsonValue &json);

        const Section &fus() const;
        const RadioSection &radio() const;

    private:
        Section m_fus;
        RadioSection m_radio;
    };

    RadioManifest() = default;
    RadioManifest(const QByteArray &text);

    const Header &header() const;
    const FirmwareInfo &firmware() const;

private:
    Header m_header;
    FirmwareInfo m_firmware;
};

}
}

