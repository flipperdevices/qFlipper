#pragma once

#include <QMap>
#include <QList>
#include <QString>
#include <QVariant>
#include <QSharedPointer>

class FileNode
{
public:
    enum class Type {
        Directory,
        RegularFile,
        Unknown
    };

    struct FileInfo {
        QString name;
        QString absolutePath;
        Type type;
        QVariant userData;

        bool operator <(const FileInfo &other) const;
    };

    using FileNodeMap = QMap<QString, QSharedPointer<FileNode>>;
    using FileInfoList = QList<FileInfo>;

    FileNode();
    FileNode(const QString &name, Type type, const QVariant &data = QVariant());

    bool operator ==(const FileNode &other) const;
    bool operator !=(const FileNode &other) const;

    const QString &name() const;
    const QString &absolutePath() const;
    Type type() const;
    const QVariant &userData() const;
    const FileInfo &fileInfo() const;

    bool addDirectory(const QString &path);
    bool addFile(const QString &path, const QVariant &data);

    FileNode *parent() const;
    FileNode *child(const QString &name) const;

    FileNode *find(const QString &path);

    FileInfoList toPreOrderList() const;
    FileInfoList difference(FileNode *other);
    FileInfoList changed(FileNode *other);

private:
    void setParent(FileNode *node);
    void addChild(const QSharedPointer<FileNode> &nodePtr);
    FileNode *traverse(const QStringList &fragments);

    FileNode *m_parent;
    FileNodeMap m_children;
    FileInfo m_info;
};
