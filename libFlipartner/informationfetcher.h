#ifndef ABSTRACTINFORMATIONFETCHER_H
#define ABSTRACTINFORMATIONFETCHER_H

#include <QObject>
#include <QRunnable>

#include "usbdeviceparams.h"

namespace Flipper {

class AbstractInformationFetcher : public QObject, public QRunnable
{
    Q_OBJECT

public:
    virtual ~AbstractInformationFetcher();

signals:
    void nameFetched(const QString);
    void targetFetched(const QString);
    void versionFetched(const QString);

};

class VCPInformationFetcher : public AbstractInformationFetcher {
    Q_OBJECT

public:
    VCPInformationFetcher(const USBDeviceParams &parameters);
    void run() override;

private:
    USBDeviceParams m_parameters;
};

}


#endif // ABSTRACTINFORMATIONFETCHER_H
