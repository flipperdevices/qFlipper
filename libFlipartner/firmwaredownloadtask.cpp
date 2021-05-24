#include "firmwaredownloadtask.h"

#include <QIODevice>
#include <QDebug>

#include "dfusedevice.h"
#include "dfusefile.h"

FirmwareDownloadTask::FirmwareDownloadTask(const FlipperInfo &info, QIODevice *file):
    m_info(info), m_file(file), m_prevProgress(-1), m_currentProgress(0)
{}

FirmwareDownloadTask::~FirmwareDownloadTask()
{}

void FirmwareDownloadTask::run()
{
    // TODO: Check for all errors (and deal with them gracefully in GUI)
    m_file->open(QIODevice::ReadOnly);

    DfuseFile fw(m_file);
    DfuseDevice dev(m_info.params);

    connect(&dev, &DfuseDevice::progressChanged, this, &FirmwareDownloadTask::onProgressChanged);
    connect(&dev, &DfuseDevice::statusChanged, this, &FirmwareDownloadTask::statusChanged);

    dev.beginTransaction();
    dev.download(&fw);
    dev.endTransaction();

    m_file->close();
    emit finished();
}

void FirmwareDownloadTask::onProgressChanged(int progress)
{
    if(m_prevProgress == progress) {
        return;
    }

    m_prevProgress = progress;
    m_currentProgress = progress / 2 + ((m_currentProgress < 49) ? 0 : 50); // Calculate total progress

    emit progressChanged(m_currentProgress);
}
