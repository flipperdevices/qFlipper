#pragma once

#include "abstractoperation.h"

namespace Flipper {
namespace Zero {

class DeviceState;
class CommandInterface;

class AbstractUtilityOperation : public AbstractOperation
{
    Q_OBJECT

public:
    AbstractUtilityOperation(CommandInterface *cli, DeviceState *deviceState, QObject *parent = nullptr);
    virtual ~AbstractUtilityOperation() {}

    void start() override;

    CommandInterface *cli() const;
    DeviceState *deviceState() const;

private slots:
    virtual void advanceOperationState() = 0;

private:
    CommandInterface *m_cli;
    DeviceState *m_deviceState;
};

}
}

