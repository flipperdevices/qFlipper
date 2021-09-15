#pragma once

#include <QMap>
#include <QString>
#include <QVariant>
#include <QSharedPointer>

class FileNode
{
    using FileNodeMap = QMap<QString, QSharedPointer<FileNode>>;

public:
    enum class Type {
        RegularFile,
        Directory,
        Unknown
    };

    struct Attributes {
        QString name;
        QString path;
        Type type;
        QVariant userData;
    };

    FileNode();
    FileNode(const QString &name, Type type, const QVariant &data = QVariant());

    const QString &name() const;
    const QString &path() const;
    Type type() const;
    const QVariant &userData() const;

    bool addDirectory(const QString &path);
    bool addFile(const QString &path, const QVariant &data);

    FileNode *traverse(const QStringList &fragments);
    FileNode *child(const QString &name) const;
    FileNode *parent() const;

    QList<Attributes> toList() const;

    void print() const;

private:
    void setParent(FileNode *node);
    void addChild(const QSharedPointer<FileNode> &nodePtr);

    FileNode *m_parent;
    FileNodeMap m_children;
    Attributes m_attributes;
};
