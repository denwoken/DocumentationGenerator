#include "DrawioExporter.h"
#include "DrawIoExecutable.h"
#include "qdebug.h"
#include "qdir.h"
#include "qfileinfo.h"

DrawioExporter::DrawioExporter(QObject *parent)
    : QObject{parent}
{}

void DrawioExporter::setDrawIoExec(DrawIoExecutable* drawioExec)
{
    m_drawioExec = drawioExec;
}

void DrawioExporter::setScale(float scale){
    m_scale = std::min(0.0f, scale);
}

float DrawioExporter::getScale() const {
    return m_scale;
}

bool DrawioExporter::exportToDrawio(const QDomDocument &doc, const QString &outputPath) const
{
    if(!outputPath.endsWith(".drawio", Qt::CaseInsensitive))
    {
        qWarning() << "exportToDrawio error, wrong extension, path: " << outputPath;
        return 1;
    }
    QFileInfo fi(outputPath);
    QDir().mkpath(fi.absolutePath());
    QFile outputFile(outputPath);
    if(!outputFile.open(QFile::WriteOnly)){
        qWarning() << "open(QFile::WriteOnly) error path:" << outputPath;
        return 1;
    }
    outputFile.resize(0);

    QTextStream out(&outputFile);
    doc.save(out, 2, QDomNode::EncodingFromDocument);
    outputFile.close();

    return 0;
}

bool DrawioExporter::exportToImage(const QString &drawioPath, const QString &outImagePath) const
{
    if(!m_drawioExec || !m_drawioExec->isValid()){
        qWarning() << "drawio to image exporting error, caused by invalidity of DrawIoExecutable";
        return 1;
    }


    QFileInfo fi(outImagePath);
    QDir().mkpath(fi.absolutePath());
    QString format = fi.suffix();
    if(format.isEmpty() ||
        !QList<QString>({"jpg", "jpeg", "png"}).contains(format.toLower())){
        qWarning() << "Wrong or unsupported format, format:" << format;
    }

    QStringList arguments;
    arguments << "--export";
    arguments << "--scale" << QString::number(m_scale);
    //    arguments << "--disable-gpu";
    arguments << "--no-sandbox";
    //arguments << "--background" << "#ffffff";
    arguments << "--format" << format;
    arguments << "--output" << /*"\"" +*/ fi.absoluteFilePath() /*+ "\""*/;
    arguments << /*"\"" +*/ QFileInfo(drawioPath).absoluteFilePath() /*+ "\""*/;

    qDebug() << "run draw.io.exe with arguments: " << arguments.join(" ");

    QProcess process;
    process.start(m_drawioExec->getExecPath(), arguments);

    if (!process.waitForFinished(3000)) {
        qCritical() << "draw.io version check timeout";
        //someErrorOccured();
        return 1;
    }
    if (process.exitStatus() != QProcess::NormalExit || process.exitCode() != 0) {
        qCritical() << "Drawio process failed:" << process.readAllStandardError();
        //someErrorOccured();
        return 1;
    }
    qDebug() << process.readAllStandardOutput();
    return 0;
}
