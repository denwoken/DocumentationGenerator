#include "CustomStatusBar.h"
#include "qlabel.h"


CustomStatusBar::CustomStatusBar(QWidget *parent):
    QStatusBar(parent)
{
    m_label = new QLabel(this);
    m_label->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);
    m_label->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    m_label->setWordWrap(false);
    addWidget(m_label, /*stretch=*/1);
}

void CustomStatusBar::displayLogLine(QtMsgType type, const QString &msg){
    QString message;
    switch(type){
    case QtDebugMsg:
        message += "Debug";
        setTextColor(QColor(0, 0xe6, 0x73));
        break;
    case QtWarningMsg:
        message += "Warning";
        setTextColor(QColor(0xff, 0xff, 0x3c));
        break;
    case QtCriticalMsg:
        message += "Critical";
        setTextColor(QColor(0xff, 0x4b, 0x4b));
        break;
    case QtFatalMsg:
        message += "Fatal";
        setTextColor(QColor(0xff, 0, 0));
        break;
    case QtInfoMsg:
        message += "Info";
        setTextColor(QColor(0, 0xe6, 0x73));
        break;
    default:
        break;
    }
    message += " : " + msg;
    setText(message);
}

void CustomStatusBar::setTextColor(const QColor &color){
    m_label->setStyleSheet(QString("color: %1;").arg(color.name()));
}
void CustomStatusBar::setText(const QString &text){
    m_label->setText(text);
}
