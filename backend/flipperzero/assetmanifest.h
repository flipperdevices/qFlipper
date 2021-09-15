#pragma once

#include <QByteArray>
#include <QStringList>
#include <QSharedPointer>

#include "filenode.h"
#include "failable.h"

namespace Flipper {
namespace Zero {

class AssetManifest : public Failable
{
public:
    struct FileInfo {
        qint64 size;
        QByteArray md5;
    };

    AssetManifest(const QByteArray &text);

    int version() const;
    time_t timestamp() const;
    const FileNode *tree() const;

    void print() const;

private:
    bool parseLine(const QString &line);
    bool parseVersion(const QStringList &tokens);
    bool parseTime(const QStringList &tokens);
    bool parseFile(const QStringList &tokens);
    bool parseDirectory(const QStringList &tokens);

    int m_version;
    time_t m_timestamp;
    QSharedPointer<FileNode> m_root;
};

}
}

Q_DECLARE_METATYPE(Flipper::Zero::AssetManifest::FileInfo)
