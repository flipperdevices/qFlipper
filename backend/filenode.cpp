#include "filenode.h"

#include <QQueue>

#include "debug.h"

FileNode::FileNode():
    m_parent(nullptr)
{}

FileNode::FileNode(const QString &name, Type type, const QVariant &data):
    m_parent(nullptr),
    m_info({name, QString(), type, data})
{}

bool FileNode::operator ==(const FileNode &other) const
{
    return (m_info.type == other.m_info.type) &&
           (m_info.absolutePath == other.m_info.absolutePath) &&
           (m_info.userData == other.m_info.userData);
}

bool FileNode::operator !=(const FileNode &other) const
{
    return !(*this == other);
}

const QString &FileNode::name() const
{
    return m_info.name;
}

const QString &FileNode::absolutePath() const
{
    return m_info.absolutePath;
}

FileNode::Type FileNode::type() const
{
    return m_info.type;
}

const QVariant &FileNode::userData() const
{
    return m_info.userData;
}

const FileNode::FileInfo &FileNode::fileInfo() const
{
    return m_info;
}

void FileNode::addChild(const QSharedPointer<FileNode> &nodePtr)
{
    nodePtr->setParent(this);
    m_children.insert(nodePtr->name(), nodePtr);
}

bool FileNode::addDirectory(const QString &path)
{
    auto fragments = path.split('/');
    QSharedPointer<FileNode> node(new FileNode(fragments.takeLast(), Type::Directory));

    auto *parent = traverse(fragments);
    check_return_bool(parent, QStringLiteral("No parent node found for %1.").arg(path));

    parent->addChild(node);
    return true;
}

bool FileNode::addFile(const QString &path, const QVariant &data)
{
    auto fragments = path.split('/');
    QSharedPointer<FileNode> node(new FileNode(fragments.takeLast(), Type::RegularFile, data));

    auto *parent = traverse(fragments);
    check_return_bool(parent, QStringLiteral("No parent node found for %1.").arg(path));

    parent->addChild(node);
    return true;
}

FileNode *FileNode::traverse(const QStringList &fragments)
{
    if(fragments == QStringList({QString()})) {
        return this;
    }

    auto *current = this;

    for(const auto &fragment: fragments) {
        current = current->child(fragment);

        if(!current) {
            break;
        }
    }

    return current;
}

FileNode *FileNode::child(const QString &name) const
{
    if(!m_children.contains(name)) {
        return nullptr;
    }

    return m_children.value(name).get();
}

FileNode *FileNode::find(const QString &path)
{
    const auto fragments = path.split('/');
    return traverse(fragments);
}

FileNode *FileNode::parent() const
{
    return m_parent;
}

FileNode::FileInfoList FileNode::toPreOrderList() const
{
    FileInfoList ret;
    QQueue<const FileNode*> queue;

    queue.enqueue(this);

    while(!queue.isEmpty()) {
        const auto *current = queue.dequeue();

        for(const auto &childPtr: current->m_children) {
            queue.enqueue(childPtr.get());
        }

        ret.append(current->m_info);
    }

    return ret;
}

FileNode::FileInfoList FileNode::difference(FileNode *other)
{
    FileInfoList ret;
    QQueue<FileNode*> queue;

    queue.enqueue(other);

    while(!queue.isEmpty()) {
        auto *current = queue.dequeue();
        auto *counterpart = find(current->absolutePath());

        if(!counterpart || (counterpart->type() != current->type())) {
            ret.append(current->toPreOrderList());
            continue;
        }

        for(const auto &childPtr: qAsConst(current->m_children)) {
            queue.enqueue(childPtr.get());
        }
    }

    return ret;
}

FileNode::FileInfoList FileNode::changed(FileNode *other)
{
    FileInfoList ret;
    QQueue<FileNode*> queue;

    queue.enqueue(other);

    while(!queue.isEmpty()) {
        auto *current = queue.dequeue();
        auto *counterpart = find(current->absolutePath());

        if(!counterpart || (counterpart->type() != current->type())) {
            continue;

        } else if (*current != *counterpart) {
            ret.append(current->m_info);
            continue;
        }

        for(const auto &childPtr: qAsConst(current->m_children)) {
            queue.enqueue(childPtr.get());
        }
    }

    return ret;
}

void FileNode::setParent(FileNode *node)
{
    m_parent = node;
    QStringList fragments;

    auto *current = this;
    while(current->parent()) {
        fragments.append(current->name());
        current = current->parent();
    }

    std::reverse(fragments.begin(), fragments.end());
    m_info.absolutePath = fragments.join('/');
}

bool FileNode::FileInfo::operator <(const FileInfo &other) const
{
    return (type < other.type) || (absolutePath < other.absolutePath);
}
