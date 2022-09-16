#pragma once

#include <QUrl>
#include <QList>
#include <QPointer>
#include <QStringList>
#include <QAbstractListModel>

#include "failable.h"
#include "fileinfo.h"

class QTimer;
class QFileInfo;
class AbstractOperation;

namespace Flipper {

class FlipperZero;

namespace Zero {

class FileManager : public QAbstractListModel, public Failable
{
    Q_OBJECT
    Q_PROPERTY(bool isBusy READ isBusy NOTIFY isBusyChanged)
    Q_PROPERTY(bool isRoot READ isRoot NOTIFY currentPathChanged)
    Q_PROPERTY(bool canGoBack READ canGoBack NOTIFY currentPathChanged)
    Q_PROPERTY(bool canGoForward READ canGoForward NOTIFY currentPathChanged)
    Q_PROPERTY(QString currentPath READ currentPath NOTIFY currentPathChanged)
    Q_PROPERTY(int newDirectoryIndex READ newDirectoryIndex NOTIFY newDirectoryIndexChanged)

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

    Q_INVOKABLE void cd(const QString &dirName);

    Q_INVOKABLE void historyForward();
    Q_INVOKABLE void historyBack();

    Q_INVOKABLE void rename(const QString &oldName, const QString &newName);
    Q_INVOKABLE void remove(const QString &fileName, bool recursive = false);

    Q_INVOKABLE void beginMkDir();
    Q_INVOKABLE void commitMkDir(const QString &dirName);

    Q_INVOKABLE void upload(const QList<QUrl> &urlList);
    Q_INVOKABLE void uploadTo(const QString &remoteDirName, const QList<QUrl> &urlList);
    Q_INVOKABLE void download(const QString &remoteFileName, const QUrl &localUrl, bool recursive = false);

    Q_INVOKABLE bool isTooLarge(const QList<QUrl> &urlList) const;

    // Properties
    bool isBusy() const;
    bool isRoot() const;
    bool canGoBack() const;
    bool canGoForward() const;
    QString currentPath() const;
    int newDirectoryIndex() const;

    // QAbstractListModel API
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

signals:
    void isBusyChanged();
    void currentPathChanged();
    void newDirectoryIndexChanged();
    void errorOccured();

private slots:
    void onBusyTimerTimeout();

private:
    bool checkDevice();

    void setBusy(bool busy);
    void setNewDirectoryIndex(int newIndex);

    void pushd(const QString &dirName);
    void popd();

    void listCurrentPath();

    void downloadFile(const QByteArray &remoteFileName, const QString &localFileName);
    void downloadDirectory(const QByteArray &remoteDirName, const QString &localDirName);

    void setModelDataRoot();
    void setModelData(const FileInfoList &newData);
    void registerOperation(AbstractOperation *operation);

    const QByteArray remoteFilePath(const QString &fileName) const;

    static qint64 localFileSize(const QUrl &localUrl);

    QPointer<FlipperZero> m_device;
    FileInfoList m_modelData;
    QStringList m_history;
    QStringList m_forwardHistory;
    QTimer *m_busyTimer;

    bool m_isBusy;
    bool m_hasSDCard;

    int m_newDirectoryIndex;
};

}
}

