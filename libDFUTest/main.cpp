#include <QCoreApplication>
#include <QByteArray>
#include <QDebug>
#include <QFile>

#include "usbdeviceinfo.h"
#include "dfudevice.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QFile downloadTest("full.bin"), uploadTest("upload.bin");

    if(!downloadTest.open(QIODevice::ReadOnly)) {
        qCritical() << "Can't open file for reading";
        return -1;
    }

    if(!uploadTest.open(QIODevice::WriteOnly)) {
        qCritical() << "Can't open file for writing";
        return -1;
    }

    static const USBDeviceInfo::MatchList matches = {
        {
            0x0483,
            0xdf11,
            "",
            "",
            ""
        },

        {
            0x0483,
            0x5740,
            "Flipper Devices Inc.",
            "Flipper Control Virtual ComPort",
            ""
        }
    };

    const auto devices = USBDeviceInfo::availableDevices(matches);

    if(devices.isEmpty()) {
        qDebug() << "No suitable devices found";
        return 0;
    }

    for(const auto &dev : devices) {
        qInfo() << QString("Bus:%1 Addr:%2 VID:%3 PID:%4")
                   .arg(dev.busNumber(), 3, 10, QLatin1Char('0'))
                   .arg(dev.address(), 3, 10, QLatin1Char('0'))
                   .arg(dev.vendorIdentifier(), 4, 16, QLatin1Char('0'))
                   .arg(dev.productIdentifier(), 4, 16, QLatin1Char('0'));

        if(!dev.manufacturer().isEmpty()) {
            qInfo() << "Manufacturer:" << dev.manufacturer();
        }

        if(!dev.description().isEmpty()) {
            qInfo() << "Description:" << dev.description();
        }
    }

    DFUDevice dev(devices.first());

    if(!dev.beginTransaction()) {
        return -1;
    }

//    dev.download(downloadTest, 0x08000000);
    dev.upload(uploadTest, 0x08000000, 138240);

    dev.endTransaction();

    downloadTest.close();
    uploadTest.close();

    return a.exec();

}
