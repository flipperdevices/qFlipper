#ifndef ABSTRACTFIRMWAREOPERATION_H
#define ABSTRACTFIRMWAREOPERATION_H

#include <QString>

class AbstractFirmwareOperation {
public:
    virtual ~AbstractFirmwareOperation() {}

    virtual const QString name() const = 0;
    virtual bool execute() = 0;
};

#endif // ABSTRACTFIRMWAREOPERATION_H
