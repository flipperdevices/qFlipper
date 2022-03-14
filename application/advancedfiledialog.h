#pragma once

#include <QObject>
#include <QFileDialog>

class AdvancedFileDialog: public QFileDialog
{
    Q_OBJECT
    Q_PROPERTY(QString defaultFileName READ defaultFileName WRITE setDefaultFileName NOTIFY defaultFileNameChanged)
    Q_PROPERTY(QString title READ windowTitle WRITE setWindowTitle NOTIFY titleChanged)
    Q_PROPERTY(QStringList nameFilters READ nameFilters WRITE setNameFilters NOTIFY nameFiltersChanged)
    Q_PROPERTY(StandardLocation openLocation READ openLocation WRITE setOpenLocation NOTIFY openLocationChanged)
    Q_PROPERTY(QList<QUrl> fileUrls READ selectedUrls CONSTANT)
    Q_PROPERTY(bool selectFolder READ isSelectFolder WRITE setSelectFolder NOTIFY isSelectFolderChanged)
    Q_PROPERTY(bool selectMultiple READ isSelectMultiple WRITE setSelectMultiple NOTIFY isSelectMultipleChanged)
    Q_PROPERTY(bool selectExisting READ isSelectExisting WRITE setSelectExisting NOTIFY isSelectExistingChanged)

public:
    enum StandardLocation {
        HomeLocation,
        DownloadsLocation,
        PicturesLocation,
        DocumentsLocation,
        DefaultLocation
    };

    Q_ENUM(StandardLocation)

    AdvancedFileDialog();

    void setDefaultFileName(const QString &fileName);
    QString defaultFileName() const;

    void setSelectFolder(bool set);
    bool isSelectFolder() const;

    void setSelectMultiple(bool set);
    bool isSelectMultiple() const;

    void setSelectExisting(bool set);
    bool isSelectExisting() const;

    void setOpenLocation(StandardLocation location);
    StandardLocation openLocation() const;

signals:
    void defaultFileNameChanged();
    void titleChanged();
    void nameFiltersChanged();
    void folderChanged();
    void isSelectFolderChanged();
    void isSelectMultipleChanged();
    void isSelectExistingChanged();
    void openLocationChanged();

private:
    bool m_isSelectFolder;
    bool m_isSelectMultiple;
    bool m_isSelectExisting;
};

