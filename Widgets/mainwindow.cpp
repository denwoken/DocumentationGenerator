#include "mainwindow.h"
#include "DialogHelp.h"
#include "DrawioRenderWidget.h"
#include "StatusLamp.h"
#include "qdir.h"
#include "qfiledialog.h"
#include "qlocale.h"
#include "qprocess.h"
#include "qregularexpression.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QKeyEvent>
#include "Logging.h"
#include "LogConsoleWidget.h"
#include "DrawIoExecutable.h"



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




    setStyleSheet(styleSheet() + "QToolBar{ background-color: rgb(50, 50, 50);}");




    ui->drawioExecStatus->setLampState(LampState::LampStateOff);
    ui->horizontalLayout_drawioPath->addWidget(ui->drawioExecStatus);

    m_drawIoExecutable = new DrawIoExecutable(this);

    connect(m_drawIoExecutable, &DrawIoExecutable::sigStateValidityChanged, this, [this](LampState state){
        ui->drawioExecStatus->setLampState(state);
        if(m_drawIoExecutable->version().isEmpty())
            ui->label_drawioVersion->setText("v*.*.*");
        else
            ui->label_drawioVersion->setText("v"+m_drawIoExecutable->version());
    });

    connect(ui->lineEdit_drawIoPath, &QLineEdit::editingFinished, this, &MainWindow::updateDrawIoPath);
    updateDrawIoPath();







    ui->toolBar->setMovable(false);

    auto group = new QActionGroup(this);
    group->setExclusive(true);

    ui->actionDrawioRender->setCheckable(true);
    ui->actionSettings->setCheckable(true);
    ui->actionDrawioDiagramBuilder->setCheckable(true);

    group->addAction(ui->actionDrawioRender);
    group->addAction(ui->actionSettings);
    group->addAction(ui->actionDrawioDiagramBuilder);


    ui->actionSettings->setChecked(true);
    ui->stackedWidget->setCurrentIndex(0);

    connect(ui->actionSettings, &QAction::triggered, this, [this]{ ui->stackedWidget->setCurrentIndex(0); });
    connect(ui->actionDrawioRender, &QAction::triggered, this, [this]{ ui->stackedWidget->setCurrentIndex(1); });
    connect(ui->actionDrawioDiagramBuilder, &QAction::triggered, this, [this]{ ui->stackedWidget->setCurrentIndex(2); });


    connect(ui->actionAbout, &QAction::triggered, this, [this]{
        DialogHelp* dialog = new DialogHelp(this);
        dialog->setAttribute(Qt::WA_DeleteOnClose);
        dialog->exec();
    });






    ui->drawioRenderWidget->setDrawioExec(m_drawIoExecutable);
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

void MainWindow::on_pushButton_drawIoDialog_clicked()
{
    QString fileType = tr("draw.io.executable(*.io.exe);;All Files(*.*)");
    QFileDialog* fileDialog = new QFileDialog(this);
    fileDialog->setFileMode(QFileDialog::FileMode::ExistingFile);
    fileDialog->setAcceptMode(QFileDialog::AcceptMode::AcceptOpen);
    fileDialog->setViewMode(QFileDialog::ViewMode::Detail);
    fileDialog->setNameFilter(fileType);

    QFileInfo fileinfo(ui->lineEdit_drawIoPath->text());
    fileDialog->setDirectory(fileinfo.dir());
    fileDialog->selectFile(fileinfo.absoluteFilePath());

    if(fileDialog->exec()){
        auto files = fileDialog->selectedFiles();
        if(files.size()){
            ui->lineEdit_drawIoPath->setText(files[0]);
            updateDrawIoPath();
        }
    }
}


void MainWindow::updateDrawIoPath()
{
    QString drawIoPath = ui->lineEdit_drawIoPath->text();
    m_drawIoExecutable->setDrawIOPath(drawIoPath);
}
