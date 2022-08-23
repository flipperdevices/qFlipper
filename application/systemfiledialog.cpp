#include "systemfiledialog.h"

#include <QFileDialog>
#include <QStandardPaths>

SystemFileDialog::SystemFileDialog():
    QObject(),
    m_dialog(nullptr)
{}

SystemFileDialog::~SystemFileDialog()
{
    if(m_dialog) {
        m_dialog->deleteLater();
    }
}

void SystemFileDialog::beginOpenFiles(StandardLocation openLocation, const QStringList &nameFilters)
{
    beginOpen(openLocation, QFileDialog::AcceptOpen, QFileDialog::ExistingFiles, nameFilters);
}

void SystemFileDialog::beginOpenFile(StandardLocation openLocation, const QStringList &nameFilters)
{
    beginOpen(openLocation, QFileDialog::AcceptOpen, QFileDialog::ExistingFile, nameFilters);
}

void SystemFileDialog::beginOpenDir(StandardLocation openLocation)
{
    beginOpen(openLocation, QFileDialog::AcceptOpen, QFileDialog::Directory);
}

void SystemFileDialog::beginSaveFile(StandardLocation openLocation, const QStringList &nameFilters, const QString &defaultFileName)
{
    beginOpen(openLocation, QFileDialog::AcceptSave, QFileDialog::AnyFile, nameFilters, defaultFileName);
}

void SystemFileDialog::beginSaveDir(StandardLocation openLocation)
{
    beginOpen(openLocation, QFileDialog::AcceptSave, QFileDialog::Directory);
}

void SystemFileDialog::close()
{
    if(m_dialog && m_dialog->isVisible()) {
        m_dialog->reject();
    }
}

bool SystemFileDialog::isOpen() const
{
    return m_dialog;
}

QUrl SystemFileDialog::fileUrl() const
{
    return fileUrls().isEmpty() ? QUrl() : fileUrls().at(0);
}

QList<QUrl> SystemFileDialog::fileUrls() const
{
    return m_dialog ? m_dialog->selectedUrls() : QList<QUrl>();
}

QString SystemFileDialog::selectedNameFilter() const
{
    return m_dialog ? m_dialog->selectedNameFilter() : QString();
}

void SystemFileDialog::onFileDialogAccepted()
{
    emit accepted();
}

void SystemFileDialog::onFileDialogFinished()
{
    emit finished();
    disconnect();
}

QString SystemFileDialog::standardLocationPath(StandardLocation location)
{
    if(location == HomeLocation) {
        return QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    } else if(location == DownloadsLocation) {
        return QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
    } else if(location == PicturesLocation) {
        return QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);
    } else if(location == DocumentsLocation) {
        return QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    } else {
        return QString();
    }
}

void SystemFileDialog::beginOpen(StandardLocation openLocation, int acceptMode, int fileMode, const QStringList &nameFilters, const QString &defaultFileName)
{
    if(m_dialog) {
        m_dialog->deleteLater();
    }

    m_dialog = new QFileDialog();
    connect(m_dialog, &QFileDialog::accepted, this, &SystemFileDialog::onFileDialogAccepted);
    connect(m_dialog, &QFileDialog::finished, this, &SystemFileDialog::onFileDialogFinished);

    m_dialog->setFileMode(static_cast<QFileDialog::FileMode>(fileMode));
    m_dialog->setAcceptMode(static_cast<QFileDialog::AcceptMode>(acceptMode));
    m_dialog->setDirectory(standardLocationPath(openLocation));
    m_dialog->setOption(QFileDialog::ShowDirsOnly, fileMode == QFileDialog::Directory);

    if(!defaultFileName.isEmpty()) {
        m_dialog->selectFile(defaultFileName);
    }

    if(!nameFilters.isEmpty()) {
        m_dialog->setNameFilters(nameFilters);
    }

    m_dialog->exec();
}
