#ifndef FLIPPERINFOTASK_H
#define FLIPPERINFOTASK_H

#include <QRunnable>
#include <QObject>

#include "flipperinfo.h"

class FlipperInfoTask : public QObject, public QRunnable
{
    Q_OBJECT

public:
    FlipperInfoTask(const FlipperInfo &info);

    void run() override;

signals:
    void finished(const FlipperInfo);

private:
    void getInfoNormalMode();
    void getInfoDFUMode();

    void parseHWInfo(const QByteArray &buf);
    void parseVersion(const QByteArray &buf);

    static QByteArray getValue(const QByteArray &buf, const QByteArray &token);

    FlipperInfo m_info;
};

#endif // FLIPPERINFOTASK_H
