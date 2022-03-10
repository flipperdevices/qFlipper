#include "advancedfiledialog.h"

#include <QStandardPaths>

AdvancedFileDialog::AdvancedFileDialog():
    QFileDialog()
{}

QString AdvancedFileDialog::defaultFileName() const
{
    return QString();
}

void AdvancedFileDialog::setDefaultFileName(const QString &fileName)
{
    selectFile(fileName);
}

void AdvancedFileDialog::setSelectFolder(bool set)
{
    setOption(ShowDirsOnly, set);
}

bool AdvancedFileDialog::isSelectFolder() const
{
    return testOption(ShowDirsOnly);
}

void AdvancedFileDialog::setSelectMultiple(bool set)
{
    setFileMode(isSelectExisting() ? (set ? ExistingFiles : ExistingFile) : AnyFile);
}

bool AdvancedFileDialog::isSelectMultiple() const
{
    return fileMode() == ExistingFiles;
}

void AdvancedFileDialog::setSelectExisting(bool set)
{
    setAcceptMode(set ? AcceptOpen : AcceptSave);

    if(!isSelectMultiple()) {
        setFileMode(set ? ExistingFile : AnyFile);
    }
}

bool AdvancedFileDialog::isSelectExisting() const
{
    const auto mode = fileMode();
    return (mode == ExistingFile || mode == ExistingFiles) && (acceptMode() == AcceptOpen);
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

AdvancedFileDialog::StandardLocation AdvancedFileDialog::openLocation() const
{
    // Does nothing. Is here just to keep the compiler happy.
    return DefaultLocation;
}
