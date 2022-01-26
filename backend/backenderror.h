#pragma once

#include <QObject>

class BackendError {
    Q_GADGET

public:
    enum ErrorType {
        InternetError,  // Cannot connect to update server or the server replied with error
        InvalidDevice,  // Cannot determine device type
        SerialError,    // Cannot open the serial port (access/driver issues)
        RecoveryError,  // Cannot open the DFU device (access/driver issues)
        ProtocolError,  // Received an error, unexpected or not implemented RPC response
        DiskError,      // Cannot read/write to the disk
        UnknownError    // ¯\_(ツ)_/¯
    };

    Q_ENUM(ErrorType)
};
