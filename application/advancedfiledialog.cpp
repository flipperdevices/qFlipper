#include "advancedfiledialog.h"

#include <QStandardPaths>

AdvancedFileDialog::AdvancedFileDialog():
    QFileDialog()
{}

void AdvancedFileDialog::beginOpenFiles(StandardLocation openLocation, const QStringList &nameFilters)
{
    setAcceptMode(AcceptOpen);
    setFileMode(ExistingFiles);
    beginOpen(openLocation, nameFilters);
}

void AdvancedFileDialog::beginOpenFile(StandardLocation openLocation, const QStringList &nameFilters)
{
    setAcceptMode(AcceptOpen);
    setFileMode(ExistingFile);
    beginOpen(openLocation, nameFilters);
}

void AdvancedFileDialog::beginOpenDir(StandardLocation openLocation, const QStringList &nameFilters)
{
    setAcceptMode(AcceptOpen);
    setFileMode(Directory);
    beginOpen(openLocation, nameFilters, QStringLiteral(" "), true);
}

void AdvancedFileDialog::beginSaveFile(StandardLocation openLocation, const QStringList &nameFilters, const QString &defaultFileName)
{
    setAcceptMode(AcceptSave);
    setFileMode(AnyFile);
    beginOpen(openLocation, nameFilters, defaultFileName);
}

void AdvancedFileDialog::beginSaveDir(StandardLocation openLocation, const QStringList &nameFilters)
{
    setAcceptMode(AcceptSave);
    setFileMode(Directory);
    beginOpen(openLocation, nameFilters, QStringLiteral(" "), true);
}

void AdvancedFileDialog::setOpenLocation(StandardLocation location)
{
    QString path;

    if(location == HomeLocation) {
        path = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    } else if(location == DownloadsLocation) {
        path = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
    } else if(location == PicturesLocation) {
        path = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);
    } else if(location == DocumentsLocation) {
        path = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    }

    setDirectory(path);
}

QUrl AdvancedFileDialog::fileUrl() const
{
    if(selectedUrls().isEmpty()) {
        return QUrl();
    } else {
        return selectedUrls().at(0);
    }
}

void AdvancedFileDialog::beginOpen(StandardLocation openLocation, const QStringList &nameFilters, const QString &defaultFileName, bool onlyDirs)
{
    selectFile(defaultFileName);
    setNameFilters(nameFilters);
    setOpenLocation(openLocation);
    setOption(ShowDirsOnly, onlyDirs);
    exec();
//    disconnect();
}
