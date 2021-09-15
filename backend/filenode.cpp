#include "filenode.h"

#include "macros.h"

FileNode::FileNode():
    m_type(Type::Unknown),
    m_parent(nullptr)
{}

FileNode::FileNode(const QString &name, Type type, const QVariant &data):
    m_name(name),
    m_type(type),
    m_data(data),
    m_parent(nullptr)
{}

const QString &FileNode::name() const
{
    return m_name;
}

FileNode::Type FileNode::type() const
{
    return m_type;
}

const QVariant &FileNode::data() const
{
    return m_data;
}

void FileNode::addChild(const QSharedPointer<FileNode> &node)
{
    node->setParent(this);
    m_children.insert(node->name(), node);
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

QString FileNode::path() const
{
    QStringList fragments;

    auto *current = this;
    while(current->parent()) {
        fragments.append(current->name());
        current = current->parent();
    }

    std::reverse(fragments.begin(), fragments.end());
    return fragments.join('/');
}

void FileNode::print() const
{
    qDebug() << path();

    for(const auto &node: m_children) {
        node->print();
    }
}

void FileNode::setParent(FileNode *node)
{
    m_parent = node;
}
