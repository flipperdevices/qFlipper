#ifndef ABSTRACTFIRMWAREOPERATION_H
#define ABSTRACTFIRMWAREOPERATION_H

#include <QString>
#include <QObject>

class AbstractFirmwareOperation: public QObject {
    Q_OBJECT

public:
    explicit AbstractFirmwareOperation(QObject *parent = nullptr):
        QObject(parent) {}
    virtual ~AbstractFirmwareOperation() {}

    virtual const QString name() const = 0;
    virtual bool execute() = 0;
};

#endif // ABSTRACTFIRMWAREOPERATION_H
