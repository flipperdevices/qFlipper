#ifndef FIRMWAREOPERATION_H
#define FIRMWAREOPERATION_H

#include <QString>

class FirmwareOperation {
public:
    virtual ~FirmwareOperation() {}

    virtual const QString name() const = 0;
    virtual bool execute() = 0;
};

#endif // FIRMWAREOPERATION_H
