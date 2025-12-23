#include "mainwindow.h"
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




    m_drawioPathStatus = new StatusLamp(this);
    m_drawioPathStatus->setLampState(LampState::LampStateOff);
    ui->horizontalLayout_drawioPath->addWidget(m_drawioPathStatus);


    connect(ui->lineEdit_drawIoPath, &QLineEdit::editingFinished, this, &MainWindow::updateDrawIoPath);

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

QString parseDrawIoVersion(const QString& text)
{
    static QRegularExpression re(R"((\d+\.\d+\.\d+))");
    auto match = re.match(text);
    return match.hasMatch() ? match.captured(1) : QString();
}
void MainWindow::updateDrawIoPath()
{
    QString drawIoPath = ui->lineEdit_drawIoPath->text();
    m_drawioPathStatus->setLampStateMedium();

    QFileInfo info(drawIoPath);
    if (!info.exists() || !info.isExecutable()) {
        qCritical() << "Draw.io not found or not executable:" << drawIoPath;
        m_drawioPathStatus->setLampStateOff();
        return;
    }

    QProcess process;
    process.start(drawIoPath, {"--version"});

    if (!process.waitForFinished(3000)) {
        qCritical() << "draw.io version check timeout";
        m_drawioPathStatus->setLampStateOff();
        return;
    }

    if (process.exitStatus() != QProcess::NormalExit || process.exitCode() != 0) {
        qCritical() << "Process failed:" << process.readAllStandardError();
        m_drawioPathStatus->setLampStateOff();
        return;
    }

    QString output = QString::fromUtf8(process.readAllStandardOutput()).trimmed();
    qDebug() << "draw.io version output:" << output;

    QString version = parseDrawIoVersion(output);
    if (version.isEmpty()) {
        qCritical() << "Failed to parse draw.io version";
        m_drawioPathStatus->setLampStateOff();
        return;
    }

    m_drawioPath = drawIoPath;
    m_drawioVersion = version;
    m_drawioPathStatus->setLampStateOn();
}
