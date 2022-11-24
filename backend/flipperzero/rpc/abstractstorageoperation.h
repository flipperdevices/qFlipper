#pragma once

#include "abstractprotobufoperation.h"

#include <QByteArray>

namespace Flipper {
namespace Zero {

class AbstractStorageOperation : public AbstractProtobufOperation
{
    Q_OBJECT

public:
    AbstractStorageOperation(uint32_t id, const QByteArray &path, QObject *parent = nullptr);
    virtual ~AbstractStorageOperation();

protected:
    const QByteArray &path() const;

private:
    QByteArray m_path;
};

}
}

