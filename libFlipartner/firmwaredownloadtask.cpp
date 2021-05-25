#include "firmwaredownloadtask.h"

#include <QIODevice>
#include <QDebug>

#include "dfusedevice.h"
#include "dfusefile.h"

FirmwareDownloadTask::FirmwareDownloadTask(const FlipperInfo &info, QIODevice *file):
    m_info(info), m_file(file)
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

    m_info.status.message = "Updating";
    emit statusChanged(m_info);

    dev.beginTransaction();
    dev.download(&fw);
    dev.leave();
    dev.endTransaction();

    m_file->close();

    m_info.status.message = "Finished";
    emit statusChanged(m_info);

    emit finished();
}

void FirmwareDownloadTask::onProgressChanged(const int operation, const double progress)
{
    m_info.status.progress = progress / 2.0 + (operation == DfuseDevice::Download ? 50 : 0);
    emit statusChanged(m_info);
}
