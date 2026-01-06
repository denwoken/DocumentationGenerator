#pragma once

#include "StatusLamp.h"
#include <QObject>

class DrawIoExecutable : public QObject
{
    Q_OBJECT
public:
    explicit DrawIoExecutable(QObject *parent = nullptr);
    void setDrawIOPath(QString drawio);

    QString getExecPath();
    bool isValid() const;
    QString version() const;
protected:

    void checkDrawIoPath();
    QString parseDrawIoVersion(const QString& text);

    void someErrorOccured();

private:
    bool m_isValid = false;
    QString m_drawioPath = "";
    QString m_drawioVersion = "";

signals:
    void sigStateValidityChanged(LampState state);
};
