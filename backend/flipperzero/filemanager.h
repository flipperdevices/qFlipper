#pragma once

#include <QStringList>
#include <QAbstractListModel>

#include "fileinfo.h"

namespace Flipper {

class FlipperZero;

namespace Zero {

class FileManager : public QAbstractListModel
{
    Q_OBJECT

public:
    enum FieldRole {
        FileName = Qt::UserRole,
        FileType,
        FileSize
    };

    Q_ENUM(FieldRole)

    FileManager(QObject *parent = nullptr);

    void setDevice(FlipperZero *device);

    // Methods to call from Qml
    Q_INVOKABLE void cd(const QString &dirName);

    // Properties
    QString currentPath() const;

    // QAbstractListModel API
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

signals:
    void currentPathChanged();

private:
    void setModelData(const FileInfoList &newData);

    FlipperZero *m_device;
    FileInfoList m_modelData;
    QStringList m_history;
};

}
}

