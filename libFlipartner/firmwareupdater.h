#ifndef FIRMWAREUPDATER_H
#define FIRMWAREUPDATER_H

#include <QObject>

class FirmwareUpdater : public QObject
{
    Q_OBJECT

public:
    FirmwareUpdater(QObject *parent = nullptr);

signals:
    void localFlashRequested();

private slots:
    void beginLocalFlash();
};

#endif // FIRMWAREUPDATER_H
