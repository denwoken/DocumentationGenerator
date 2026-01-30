#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

#include "Logging.h"

class StatusLamp;
class DrawIoExecutable;
class CustomStatusBar;
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
    CustomStatusBar* m_statusBar;
    Logging::LogConsoleWidget* m_consoleWidget;


    DrawIoExecutable* m_drawIoExecutable;


};
#endif // MAINWINDOW_H
