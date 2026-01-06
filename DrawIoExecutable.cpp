#include "DrawIoExecutable.h"
#include "qdebug.h"
#include "qfileinfo.h"
#include "qprocess.h"
#include "qregularexpression.h"

DrawIoExecutable::DrawIoExecutable(QObject *parent)
    : QObject{parent}
{}

void DrawIoExecutable::setDrawIOPath(QString drawio)
{
    m_drawioPath = drawio;
    m_drawioVersion = "";
    checkDrawIoPath();
}

QString DrawIoExecutable::getExecPath(){
    return m_drawioPath;
}

bool DrawIoExecutable::isValid()const{
    return m_isValid;
}

QString DrawIoExecutable::version() const{
    return m_drawioVersion;
}

void DrawIoExecutable::checkDrawIoPath()
{
    emit sigStateValidityChanged(LampState::LampStateMedium);
    m_isValid = false;

    QFileInfo info(m_drawioPath);
    if (!info.exists() || !info.isExecutable()) {
        qCritical() << "Draw.io not found or not executable:" << m_drawioPath;
        someErrorOccured();
        return;
    }

    QProcess process;
    process.start(m_drawioPath, {"--version"});

    if (!process.waitForFinished(3000)) {
        qCritical() << "draw.io version check timeout";
        someErrorOccured();
        return;
    }

    if (process.exitStatus() != QProcess::NormalExit || process.exitCode() != 0) {
        qCritical() << "Process failed:" << process.readAllStandardError();
        someErrorOccured();
        return;
    }

    QString output = QString::fromUtf8(process.readAllStandardOutput()).trimmed();
    qDebug() << "draw.io version output:" << output;

    QString version = parseDrawIoVersion(output);
    if (version.isEmpty()) {
        qCritical() << "Failed to parse draw.io version";
        someErrorOccured();
        return;
    }

    m_drawioVersion = version;
    emit sigStateValidityChanged(LampState::LampStateOn);
    m_isValid = true;
}

QString DrawIoExecutable::parseDrawIoVersion(const QString &text)
{
    static QRegularExpression re(R"((\d+\.\d+\.\d+))");
    auto match = re.match(text);
    return match.hasMatch() ? match.captured(1) : QString();
}

void DrawIoExecutable::someErrorOccured()
{
    m_drawioPath = m_drawioVersion = "";
    emit sigStateValidityChanged(LampState::LampStateOff);
}
