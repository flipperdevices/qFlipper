#include "guiscreenframeoperation.h"

#include <QTimer>
#include <QSerialPort>

#include "flipperzero/protobuf/guiprotobufmessage.h"

using namespace Flipper;
using namespace Zero;

GuiScreenFrameOperation::GuiScreenFrameOperation(QSerialPort *serialPort, const QByteArray &screenData, QObject *parent):
    AbstractProtobufOperation(serialPort, parent),
    m_screenData(screenData)
{}

const QString GuiScreenFrameOperation::description() const
{
    return QStringLiteral("Send screen frame @%1").arg(serialPort()->portName());
}

bool GuiScreenFrameOperation::begin()
{
    GuiScreenFrameRequest request(serialPort(), m_screenData);
    const auto success = request.send();

    if(success) {
        QTimer::singleShot(0, this, &AbstractOperation::finish);
    }

    return success;
}
