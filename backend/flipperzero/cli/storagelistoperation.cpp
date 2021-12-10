#include "storagelistoperation.h"

#include "flipperzero/protobuf/storageprotobufmessage.h"

using namespace Flipper;
using namespace Zero;

StorageListOperation::StorageListOperation(QSerialPort *serialPort, const QByteArray &path, QObject *parent):
    AbstractSerialOperation(serialPort, parent),
    m_path(path)
{}

const QString StorageListOperation::description() const
{
    return QStringLiteral("Storage List @%1").arg(QString(m_path));
}

const FileInfoList &StorageListOperation::files() const
{
    return m_result;
}

#include <QDebug>
void StorageListOperation::onSerialPortReadyRead()
{
    StorageListResponse response(serialPort());

    while(response.receive()) {

        if(!response.isOk()) {
            finishWithError(QStringLiteral("Device replied with an error response"));
            return;

        } else if(!response.isValidType()) {
            finishWithError(QStringLiteral("Expected STORAGE LIST reply, got something else"));
            return;
        }

        const auto &files = response.files();
        for(auto &file : files) {
            qDebug() << file.name;

            FileInfo fileInfo {
                .name = QByteArray(file.name),
                .absolutePath = QByteArray(),
                .type = file.type == PB_Storage_File_FileType_FILE ? FileType::RegularFile : FileType::Directory,
                .size = file.size
            };

            m_result.append(fileInfo);
        }

        if(!response.hasNext()) {
            finish();
            return;
        }
    }
}

bool StorageListOperation::begin()
{
    StorageListRequest request(serialPort(), m_path);
    return request.send();
}
