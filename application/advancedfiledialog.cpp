#include "advancedfiledialog.h"

#include <QStandardPaths>

AdvancedFileDialog::AdvancedFileDialog():
    QFileDialog(),
    m_isSelectFolder(false),
    m_isSelectMultiple(false),
    m_isSelectExisting(false)
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
    if(set == m_isSelectFolder) {
        return;
    } else if(set) {
        setFileMode(Directory);
    } else {
        setFileMode(m_isSelectExisting ? (m_isSelectMultiple ? ExistingFiles : ExistingFile) : AnyFile);
    }

    setOption(ShowDirsOnly, set);
    m_isSelectFolder = set;

    emit isSelectFolderChanged();
}

bool AdvancedFileDialog::isSelectFolder() const
{
    return m_isSelectFolder;
}

void AdvancedFileDialog::setSelectMultiple(bool set)
{
    if(set == m_isSelectMultiple) {
        return;
    }

    setFileMode(m_isSelectExisting ? (set ? ExistingFiles : ExistingFile) : AnyFile);

    m_isSelectMultiple = set;
    emit  isSelectMultipleChanged();
}

bool AdvancedFileDialog::isSelectMultiple() const
{
    return m_isSelectMultiple;
}

void AdvancedFileDialog::setSelectExisting(bool set)
{
    if(set == m_isSelectExisting) {
        return;
    }

    setFileMode(set ? (m_isSelectMultiple ? ExistingFiles : ExistingFile) : AnyFile);
    setAcceptMode(set ? AcceptOpen : AcceptSave);

    m_isSelectExisting = set;
    emit isSelectExistingChanged();
}

bool AdvancedFileDialog::isSelectExisting() const
{
    return m_isSelectExisting;
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
