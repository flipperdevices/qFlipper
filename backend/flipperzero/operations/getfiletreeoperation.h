#pragma once

#include "flipperzerooperation.h"
#include "fileinfo.h"

#include <QList>

class QSerialPort;

namespace Flipper {
namespace Zero {

class GetFileTreeOperation : public Operation
{
    Q_OBJECT

    enum State {
        Running = BasicState::User
    };

public:
    using FileInfoList = QList<FileInfo>;

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
