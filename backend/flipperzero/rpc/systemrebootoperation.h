#pragma once

#include "abstractprotobufoperation.h"

namespace Flipper {
namespace Zero {

class SystemRebootOperation : public AbstractProtobufOperation
{
    Q_OBJECT

public:
    enum RebootMode {
        RebootModeOS = 0,
        RebootModeRecovery = 1,
        RebootModeUpdate = 2,
    };

    SystemRebootOperation(uint32_t id, RebootMode rebootType, QObject *parent = nullptr);
    const QString description() const override;
    const QByteArray encodeRequest(ProtobufPluginInterface *encoder) override;

private:
    RebootMode m_rebootMode;
};

}
}

