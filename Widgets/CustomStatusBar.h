#pragma once
#include <QStatusBar>
#include "Logging.h"
class QLabel;
class CustomStatusBar : public QStatusBar
{
    Q_OBJECT
public:
    explicit CustomStatusBar(QWidget* parent = nullptr);

public slots:
    void displayLogLine(QtMsgType type, const QString& msg);
protected:
    void setTextColor(const QColor& color);
    void setText(const QString &text);
private:
    QLabel* m_label;
};





