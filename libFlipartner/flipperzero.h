#ifndef FLIPPERZERO_H
#define FLIPPERZERO_H

#include <QObject>

#include "usbdeviceparams.h"

namespace Flipper {

class Zero : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString name READ name NOTIFY nameChanged)
    Q_PROPERTY(QString model READ model NOTIFY modelChanged)
    Q_PROPERTY(QString target READ target NOTIFY targetChanged)
    Q_PROPERTY(QString version READ version NOTIFY versionChanged)
    Q_PROPERTY(bool isDFU READ isDFU NOTIFY isDFUChanged)

public:
    Zero(const USBDeviceParams &parameters, QObject *parent = nullptr);

    const QString &name() const;
    const QString &model() const;
    const QString &target() const;
    const QString &version() const;
//    const QString &status() const;

    bool isDFU() const;

    void setName(const QString &name);
    void setTarget(const QString &target);
    void setVersion(const QString &version);

    void *uniqueID() const;

signals:
    void nameChanged(const QString&);
    void modelChanged(const QString&);
    void targetChanged(const QString&);
    void versionChanged(const QString&);
//    void statusChanged(const QString&) const;

    void isDFUChanged(bool);

private:
    void fetchInfoNormalMode();
    void fetchInfoDFUMode();

    USBDeviceParams m_parameters;

    QString m_name;
    QString m_target;
    QString m_version;
};

}

#endif // FLIPPERZERO_H
