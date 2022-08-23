#pragma once

#include <QUrl>
#include <QObject>

class QFileDialog;

class AdvancedFileDialog: public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString selectedNameFilter READ selectedNameFilter CONSTANT)
    Q_PROPERTY(QUrl fileUrl READ fileUrl CONSTANT)
    Q_PROPERTY(QList<QUrl> fileUrls READ fileUrls CONSTANT)

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
    ~AdvancedFileDialog();

    Q_INVOKABLE void beginOpenFiles(AdvancedFileDialog::StandardLocation openLocation, const QStringList &nameFilters);
    Q_INVOKABLE void beginOpenFile(AdvancedFileDialog::StandardLocation openLocation, const QStringList &nameFilters);
    Q_INVOKABLE void beginOpenDir(AdvancedFileDialog::StandardLocation openLocation);

    Q_INVOKABLE void beginSaveFile(AdvancedFileDialog::StandardLocation openLocation, const QStringList &nameFilters, const QString &defaultFileName);
    Q_INVOKABLE void beginSaveDir(AdvancedFileDialog::StandardLocation openLocation);

    Q_INVOKABLE void close();

    bool isOpen() const;

    QUrl fileUrl() const;
    QList<QUrl> fileUrls() const;
    QString selectedNameFilter() const;

signals:
    void finished();
    void accepted();

private slots:
    void onFileDialogAccepted();
    void onFileDialogFinished();

private:
    static QString standardLocationPath(StandardLocation location);

    void beginOpen(StandardLocation openLocation, int acceptMode, int fileMode, const QStringList &nameFilters = QStringList(), const QString &defaultFileName = QString());

    QFileDialog *m_dialog;
};
