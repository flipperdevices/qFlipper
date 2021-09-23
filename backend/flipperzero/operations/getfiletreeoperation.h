#pragma once

#include "flipperzerooperation.h"
#include "fileinfo.h"

#include <QList>

class QSerialPort;

namespace Flipper {
namespace Zero {

class ListOperation;

class GetFileTreeOperation : public Operation
{
    Q_OBJECT

    enum State {
        PreparingNextOperation = BasicState::User,
        RunningOperation
    };

public:
    using FileInfoList = QList<FileInfo>;

    GetFileTreeOperation(FlipperZero *device, const QByteArray &rootPath, QObject *parent = nullptr);
    const QString description() const override;
    const FileInfoList &result() const;

private slots:
    void transitionToNextState() override;

private:
    QByteArray m_rootPath;
    QByteArray m_currentPath;
    FileInfoList m_result;
};

}
}
