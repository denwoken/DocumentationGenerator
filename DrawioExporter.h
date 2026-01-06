#pragma once


#include "qprocess.h"
#include <QObject>
#include <QDomDocument>
class DrawIoExecutable;
class DrawioExporter : public QObject
{
    Q_OBJECT
public:
    explicit DrawioExporter(QObject *parent = nullptr);
    void setDrawIoExec(DrawIoExecutable* drawioExec);

    void setScale(float scale);
    float getScale()const;





    bool exportToDrawio(const QDomDocument &doc, const QString &outPath) const;


    bool exportToImage(const QString &drawioPath,
                       const QString &outImagePath) const;


signals:
    void exportFinished(bool ok, QString message);
private:
    DrawIoExecutable* m_drawioExec = nullptr;

    float m_scale = 1;
};


