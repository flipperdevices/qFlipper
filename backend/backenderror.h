#pragma once

#include <QObject>

class BackendError : public QObject {
    Q_OBJECT

public:
    enum ErrorType {
        // General errors
        NoError,        // No error obviously
        UnknownError,   // ¯\_(ツ)_/¯

        // Top-level errors
        InvalidDevice,   // Cannot determine device type
        InternetError,   // Cannot connect to update server or the server replied with error
        BackupError,     // Cannot perform backup
        DiskError,       // Cannot read/write to the disk
        DataError,       // Necessary files are corrupted
        SerialAccessError, // Cannot access device in Serial mode
        RecoveryAccessError, // Cannot access device in DFU mode
        OperationError,  // Current operation was interrupted
        UpdaterError, // Updater returned an error code

        // Low-level errors
        SerialError,    // Cannot open/read/write to/from serial port
        RecoveryError,  // Cannot open/read/write to/from DFU device
        ProtocolError,  // Received an error, unexpected or not implemented RPC response
        TimeoutError    // The operation took too long to execute
    };

    Q_ENUM(ErrorType)
};
