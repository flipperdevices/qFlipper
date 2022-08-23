#include "advancedfiledialog.h"

#include <QFileDialog>
#include <QStandardPaths>

AdvancedFileDialog::AdvancedFileDialog():
    QObject(),
    m_dialog(nullptr)
{}

AdvancedFileDialog::~AdvancedFileDialog()
{
    if(m_dialog) {
        m_dialog->deleteLater();
    }
}

void AdvancedFileDialog::beginOpenFiles(StandardLocation openLocation, const QStringList &nameFilters)
{
    beginOpen(openLocation, QFileDialog::AcceptOpen, QFileDialog::ExistingFiles, nameFilters);
}

void AdvancedFileDialog::beginOpenFile(StandardLocation openLocation, const QStringList &nameFilters)
{
    beginOpen(openLocation, QFileDialog::AcceptOpen, QFileDialog::ExistingFile, nameFilters);
}

void AdvancedFileDialog::beginOpenDir(StandardLocation openLocation)
{
    beginOpen(openLocation, QFileDialog::AcceptOpen, QFileDialog::Directory);
}

void AdvancedFileDialog::beginSaveFile(StandardLocation openLocation, const QStringList &nameFilters, const QString &defaultFileName)
{
    beginOpen(openLocation, QFileDialog::AcceptSave, QFileDialog::AnyFile, nameFilters, defaultFileName);
}

void AdvancedFileDialog::beginSaveDir(StandardLocation openLocation)
{
    beginOpen(openLocation, QFileDialog::AcceptSave, QFileDialog::Directory);
}

void AdvancedFileDialog::close()
{
    if(m_dialog && m_dialog->isVisible()) {
        m_dialog->reject();
    }
}

bool AdvancedFileDialog::isOpen() const
{
    return m_dialog;
}

QUrl AdvancedFileDialog::fileUrl() const
{
    return fileUrls().isEmpty() ? QUrl() : fileUrls().at(0);
}

QList<QUrl> AdvancedFileDialog::fileUrls() const
{
    return m_dialog ? m_dialog->selectedUrls() : QList<QUrl>();
}

QString AdvancedFileDialog::selectedNameFilter() const
{
    return m_dialog ? m_dialog->selectedNameFilter() : QString();
}

void AdvancedFileDialog::onFileDialogAccepted()
{
    emit accepted();
}

void AdvancedFileDialog::onFileDialogFinished()
{
    emit finished();
    disconnect();
}

QString AdvancedFileDialog::standardLocationPath(StandardLocation location)
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

void AdvancedFileDialog::beginOpen(StandardLocation openLocation, int acceptMode, int fileMode, const QStringList &nameFilters, const QString &defaultFileName)
{
    if(m_dialog) {
        m_dialog->deleteLater();
    }

    m_dialog = new QFileDialog();
    connect(m_dialog, &QFileDialog::accepted, this, &AdvancedFileDialog::onFileDialogAccepted);
    connect(m_dialog, &QFileDialog::finished, this, &AdvancedFileDialog::onFileDialogFinished);

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
