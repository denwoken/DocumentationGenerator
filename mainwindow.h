#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

// namespace Logging
// {
// class LogConsoleWidget;
// }
#include "Logging.h"

class StatusLamp;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void keyPressEvent(QKeyEvent *event);

private slots:
    void on_pushButton_drawIoDialog_clicked();

    void updateDrawIoPath();

private:
    Ui::MainWindow *ui;
    Logging::LogConsoleWidget* m_consoleWidget;

    QString m_drawioPath = "";
    QString m_drawioVersion = "";
    StatusLamp* m_drawioPathStatus = nullptr;

};
#endif // MAINWINDOW_H
