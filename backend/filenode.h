#pragma once

#include <QMap>
#include <QString>
#include <QVariant>
#include <QSharedPointer>

class FileNode
{
public:
    enum class Type {
        RegularFile,
        Directory,
        Unknown
    };

    FileNode();
    FileNode(const QString &name, Type type, const QVariant &data = QVariant());

    const QString &name() const;
    Type type() const;
    const QVariant &data() const;

    bool addDirectory(const QString &path);
    bool addFile(const QString &path, const QVariant &data);

    FileNode *traverse(const QStringList &fragments);
    FileNode *child(const QString &name) const;
    FileNode *parent() const;

    QString path() const;

    void print() const;

private:
    void setParent(FileNode *node);
    void addChild(const QSharedPointer<FileNode> &node);

    QString m_name;
    Type m_type;
    QVariant m_data;

    FileNode *m_parent;
    QMap<QString, QSharedPointer<FileNode>> m_children;
};
