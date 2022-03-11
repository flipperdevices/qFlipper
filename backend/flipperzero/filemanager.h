#pragma once

#include <QUrl>
#include <QList>
#include <QStringList>
#include <QAbstractListModel>

#include "fileinfo.h"

class QTimer;
class QFileInfo;

namespace Flipper {

class FlipperZero;

namespace Zero {

class FileManager : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(bool isBusy READ isBusy NOTIFY isBusyChanged)
    Q_PROPERTY(bool canGoBack READ canGoBack NOTIFY currentPathChanged)
    Q_PROPERTY(bool canGoForward READ canGoForward NOTIFY currentPathChanged)
    Q_PROPERTY(QString currentPath READ currentPath NOTIFY currentPathChanged)

public:
    enum FieldRole {
        FileName = Qt::UserRole,
        FilePath,
        FileType,
        FileSize
    };

    Q_ENUM(FieldRole)

    FileManager(QObject *parent = nullptr);

    void setDevice(FlipperZero *device);

    // Methods to call from Qml
    Q_INVOKABLE void reset();
    Q_INVOKABLE void refresh();

    Q_INVOKABLE void pushd(const QString &dirName);
    Q_INVOKABLE void popd();

    Q_INVOKABLE void historyForward();
    Q_INVOKABLE void historyBack();

    Q_INVOKABLE void rename(const QString &oldName, const QString &newName);
    Q_INVOKABLE void remove(const QString &fileName, bool recursive = false);

    Q_INVOKABLE void upload(const QList<QUrl> &urlList);
    Q_INVOKABLE void download(const QString &remoteFileName, const QUrl &localUrl, bool recursive = false);

    // Properties
    bool isBusy() const;
    bool canGoBack() const;
    bool canGoForward() const;
    QString currentPath() const;

    // QAbstractListModel API
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

signals:
    void isBusyChanged();
    void currentPathChanged();

private slots:
    void onBusyTimerTimeout();

private:
    void setBusy(bool busy);

    void listCurrentPath();
    void uploadFile(const QFileInfo &info);
    void uploadDirectory(const QFileInfo &info);

    void downloadFile(const QByteArray &remoteFileName, const QString &localFileName);
    void downloadDirectory(const QByteArray &remoteDirName, const QString &localDirName);

    void setModelData(const FileInfoList &newData);

    const QByteArray remoteFilePath(const QString &fileName) const;

    FlipperZero *m_device;
    FileInfoList m_modelData;
    QStringList m_history;
    QStringList m_forwardHistory;
    QTimer *m_busyTimer;
    bool m_isBusy;
};

}
}

