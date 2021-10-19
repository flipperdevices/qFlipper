#pragma once

#include "abstractutilityoperation.h"
#include "fileinfo.h"

class QSerialPort;

namespace Flipper {
namespace Zero {

class GetFileTreeOperation : public AbstractUtilityOperation
{
    Q_OBJECT

    enum State {
        Running = AbstractOperation::User
    };

public:
    GetFileTreeOperation(CommandInterface *cli, DeviceState *deviceState, const QByteArray &rootPath, QObject *parent = nullptr);
    const QString description() const override;
    const FileInfoList &result() const;

private slots:
    void advanceOperationState() override;

private:
    void listDirectory(const QByteArray &path);

    QByteArray m_rootPath;
    QByteArray m_currentPath;
    FileInfoList m_result;
    int m_pendingCount;
};

}
}
