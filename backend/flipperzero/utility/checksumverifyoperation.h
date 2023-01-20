#pragma once

#include "abstractutilityoperation.h"

#include <QUrl>
#include <QDir>
#include <QFileInfoList>

namespace Flipper {
namespace Zero {

class ChecksumVerifyOperation : public AbstractUtilityOperation
{
    Q_OBJECT

    enum State {
        VerifyingMd5Sum = AbstractOperation::User
    };

public:

    ChecksumVerifyOperation(ProtobufSession *rpc, DeviceState *deviceState, const QString &localDirectory, const QByteArray &remotePath, QObject *parent = nullptr);
    const QString description() const override;
    const QList<QUrl> &result() const;

private slots:
    void nextStateLogic() override;

private:
    void verifyMd5Sums();

    static const QByteArray calculateMd5Sum(const QFileInfo &fileInfo);

    QDir m_localDirectory;
    QByteArray m_remotePath;
    QList<QUrl> m_result;
};

}
}

