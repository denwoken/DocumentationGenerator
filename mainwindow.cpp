#include "mainwindow.h"
#include "qdir.h"
#include "qlocale.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QKeyEvent>
#include "LogConsole/Logging.h"
#include "LogConsole/LogConsoleWidget.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QFile stylesheetFile(":/Stylesheets/TolmiGrayOrangeV1.qss");
    if(stylesheetFile.open(QFile::ReadOnly | QFile::Text)){
        QTextStream in(&stylesheetFile);
        QString stylesheet = in.readAll();
        this->setStyleSheet(stylesheet);
        stylesheetFile.close();
    } else
        qDebug() << "Ошибка открытия файла:" << stylesheetFile.fileName() << stylesheetFile.errorString();




    // Виджет консоли обработчик вывода


    Logging::setEnableConsoleLogging(true);
    Logging::setEnableDebug(true);
    qInstallMessageHandler(Logging::messageHandler);

    m_consoleWidget = new Logging::LogConsoleWidget(nullptr, Qt::Window);
    m_consoleWidget->hide();
    m_consoleWidget->setStyleSheet(this->styleSheet());

    Logging::setLogConsole(m_consoleWidget);
    //connect(this, &QMainWindow::sigClosed, m_consoleWidget, &Logging::LogConsoleWidget::close);

    QString m_logFolder = QCoreApplication::applicationDirPath() + "/Logs";
    QDir logDir(m_logFolder);
    if(!logDir.exists()) QDir().mkdir(m_logFolder);

    QString logFilePath = logDir.absoluteFilePath(QDateTime::currentDateTime().toString("yyyy-MM-dd__hh-mm-ss-zzz.log"));

    // создаем файл для логов и устанавливаем его в виджет и в Logging
    m_consoleWidget->setLogFilePath(logFilePath);
    Logging::setEnableFileLogging(true);
    Logging::setLoggingFile(logFilePath);

    //загрузка настроек
    QString defSettings(logDir.absoluteFilePath("ConsoleDefaultSettings.ini"));
    if(QFileInfo::exists(defSettings))
        m_consoleWidget->loadSettings(defSettings);
    else
        m_consoleWidget->saveSettings(defSettings);  // сохраняем дефолтные настройки


}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if(event->modifiers().testFlag(Qt::KeyboardModifier::ShiftModifier) && event->key() == Qt::Key_L)
    {
        if(m_consoleWidget->isHidden())
            m_consoleWidget->show();
        else
            m_consoleWidget->hide();
    }
}
