#include "assetmanifest.h"

#include <QTextStream>

#include "macros.h"

using namespace Flipper;
using namespace Zero;

AssetManifest::AssetManifest(const QByteArray &text):
    m_version(-1),
    m_timestamp(0),
    m_root(new FileNode("", FileNode::Type::Directory))
{
    QTextStream s(text);

    auto n = 1;
    while(!s.atEnd()) {
        if(!parseLine(s.readLine())) {
            setError(QStringLiteral("Syntax error on line %1").arg(n++));
            return;
        }
    }

    if((m_version == -1) || (m_timestamp == 0)) {
        setError(QStringLiteral("Incomplete manifest file"));
    }
}

int AssetManifest::version() const
{
    return m_version;
}

time_t AssetManifest::timestamp() const
{
    return m_timestamp;
}

const FileNode *AssetManifest::tree() const
{
    return m_root.get();
}

void AssetManifest::print() const
{
    const auto list = m_root->toList();
    for(const auto &el : list) {
        if(el.userData.canConvert<FileInfo>()) {
            const auto info = el.userData.value<FileInfo>();
            qDebug() << el.path << ":" << info.size << ":" << info.md5;
        } else {
            qDebug() << el.path;
        }
    }
}

bool AssetManifest::parseLine(const QString &line)
{
    const auto tokens = line.trimmed().split(':');
    return parseFile(tokens) || parseDirectory(tokens) ||
            parseVersion(tokens) || parseTime(tokens);
}

bool AssetManifest::parseVersion(const QStringList &tokens)
{
    if((tokens.first() != "V") || (tokens.size() < 2)) {
        return false;
    }

    bool success;
    const auto version = tokens[1].toInt(&success);

    if(success) {
        m_version = version;
    }

    return success;
}

bool AssetManifest::parseTime(const QStringList &tokens)
{
    if((tokens.first() != "T") || (tokens.size() < 2)) {
        return false;
    }

    bool success;
    const auto time = tokens[1].toLongLong(&success, 10);

    if(success) {
        m_timestamp = time;
    }

    return success;
}

bool AssetManifest::parseFile(const QStringList &tokens)
{
    if((tokens.first() != "F") || (tokens.size() < 4)) {
        return false;
    }

    bool success;
    FileInfo info;

    info.md5 = tokens[1].toLocal8Bit();
    info.size = tokens[2].toLongLong(&success, 10);

    return success && m_root->addFile(tokens[3], QVariant::fromValue(info));
}

bool AssetManifest::parseDirectory(const QStringList &tokens)
{
    if((tokens.first() != "D") || (tokens.size() < 2)) {
        return false;
    }

    const auto dirName = tokens[1].endsWith('/') ? tokens[1].chopped(1) : tokens[1];
    return m_root->addDirectory(dirName);
}
