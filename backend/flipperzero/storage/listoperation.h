#pragma once

#include "simpleserialoperation.h"

#include <QList>

#include "fileinfo.h"

namespace Flipper {
namespace Zero {

class ListOperation : public SimpleSerialOperation
{
    Q_OBJECT

public:
    using FileInfoList = QList<FileInfo>;

    ListOperation(QSerialPort *serialPort, const QByteArray &dirName, QObject *parent = nullptr);
    const QString description() const override;
    const FileInfoList &result() const;

private:
    QByteArray endOfMessageToken() const override;
    QByteArray commandLine() const override;

    bool parseReceivedData() override;
    void parseDirectory(const QByteArray &line);
    void parseFile(const QByteArray &line);

private:
    QByteArray m_dirName;
    FileInfoList m_result;
};

}
}

