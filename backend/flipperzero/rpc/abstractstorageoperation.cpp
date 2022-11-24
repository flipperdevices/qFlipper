#include "abstractstorageoperation.h"

using namespace Flipper;
using namespace Zero;

AbstractStorageOperation::AbstractStorageOperation(uint32_t id, const QByteArray &path, QObject *parent):
    AbstractProtobufOperation(id, parent),
    m_path(path)
{
    // Replace forbidden characters with '_'
    for(auto it = m_path.begin(); it != m_path.end(); ++it) {
        if(QByteArrayLiteral("<>*?:|\"\\").contains(*it)) {
            *it = '_';
        }
    }
}

AbstractStorageOperation::~AbstractStorageOperation()
{}

const QByteArray &AbstractStorageOperation::path() const
{
    return m_path;
}
