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
    return QStringLiteral("Storage list @%1").arg(QString(m_path));
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
            finishWithError(QStringLiteral("Device replied with error: %1").arg(response.commandStatusString()));
            return;

        } else if(!response.isValidType()) {
            finishWithError(QStringLiteral("Expected StorageList response, got something else"));
            return;
        }

        const auto &files = response.files();
        for(auto &file : files) {

            FileInfo fileInfo {
                .name = QByteArray(file.name),
                .absolutePath = m_path + QByteArrayLiteral("/") + QByteArray(file.name),
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
