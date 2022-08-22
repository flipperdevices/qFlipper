#pragma once

#include <QFileDialog>

class AdvancedFileDialog: public QFileDialog
{
    Q_OBJECT
    Q_PROPERTY(QString selectedNameFilter READ selectedNameFilter CONSTANT)
    Q_PROPERTY(QUrl fileUrl READ fileUrl CONSTANT)
    Q_PROPERTY(QList<QUrl> fileUrls READ selectedUrls CONSTANT)

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

    Q_INVOKABLE void beginOpenFiles(AdvancedFileDialog::StandardLocation openLocation, const QStringList &nameFilters);
    Q_INVOKABLE void beginOpenFile(AdvancedFileDialog::StandardLocation openLocation, const QStringList &nameFilters);
    Q_INVOKABLE void beginOpenDir(AdvancedFileDialog::StandardLocation openLocation, const QStringList &nameFilters);

    Q_INVOKABLE void beginSaveFile(AdvancedFileDialog::StandardLocation openLocation, const QStringList &nameFilters, const QString &defaultFileName);
    Q_INVOKABLE void beginSaveDir(AdvancedFileDialog::StandardLocation openLocation, const QStringList &nameFilters);

    QUrl fileUrl() const;

private:
    void setOpenLocation(StandardLocation location);
    void beginOpen(StandardLocation openLocation, const QStringList &nameFilters, const QString &defaultFileName = QString(), bool onlyDirs = false);
};

