#pragma once

#include <QObject>

#include "failable.h"

class SignalingFailable : public QObject, public Failable
{
    Q_OBJECT

public:
    SignalingFailable(QObject *parent = nullptr):
        QObject(parent) {};

    void setError(const QString &errorMessage) override {
        Failable::setError(errorMessage);
        emit errorOccured();
    }

signals:
    void errorOccured();
};
