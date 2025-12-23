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

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void keyPressEvent(QKeyEvent *event);

private:
    Ui::MainWindow *ui;
    Logging::LogConsoleWidget* m_consoleWidget;
};
#endif // MAINWINDOW_H
