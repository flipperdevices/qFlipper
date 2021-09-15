#include "filenode.h"

#include <QQueue>

#include "macros.h"

FileNode::FileNode():
    m_parent(nullptr)
{}

FileNode::FileNode(const QString &name, Type type, const QVariant &data):
    m_parent(nullptr),
    m_attributes({name, QString(), type, data})
{}

const QString &FileNode::name() const
{
    return m_attributes.name;
}

const QString &FileNode::path() const
{
    return m_attributes.path;
}

FileNode::Type FileNode::type() const
{
    return m_attributes.type;
}

const QVariant &FileNode::userData() const
{
    return m_attributes.userData;
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

FileNode *FileNode::parent() const
{
    return m_parent;
}

QList<FileNode::Attributes> FileNode::toList() const
{
    QList<Attributes> ret;
    QQueue<const FileNode*> queue;

    queue.enqueue(this);

    while(!queue.isEmpty()) {
        const auto *current = queue.dequeue();

        for(const auto &childPtr: current->m_children) {
            queue.enqueue(childPtr.get());
        }

        ret.append(current->m_attributes);
    }

    return ret;
}

void FileNode::print() const
{
    const auto list = toList();
    for(const auto &el : list) {
        qDebug() << el.path;
    }
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
    m_attributes.path = fragments.join('/');
}
