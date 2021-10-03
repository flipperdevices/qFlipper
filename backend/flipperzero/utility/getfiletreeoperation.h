#pragma once

#include "flipperzero/flipperzerooperation.h"
#include "fileinfo.h"

class QSerialPort;

namespace Flipper {
namespace Zero {

class GetFileTreeOperation : public FlipperZeroOperation
{
    Q_OBJECT

    enum State {
        Running = BasicOperationState::User
    };

public:
    GetFileTreeOperation(FlipperZero *device, const QByteArray &rootPath, QObject *parent = nullptr);
    const QString description() const override;
    const FileInfoList &result() const;

private slots:
    void transitionToNextState() override;

private:
    void listDirectory(const QByteArray &path);

    QByteArray m_rootPath;
    QByteArray m_currentPath;
    FileInfoList m_result;
    int m_pendingCount;
};

}
}
