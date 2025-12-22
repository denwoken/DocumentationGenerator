#include "mainwindow.h"
#include "qlocale.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QFile stylesheetFile(":/SteleSheets/TolmiGrayOrangeV1.qss");
    if(stylesheetFile.open(QFile::ReadOnly | QFile::Text)){
        QTextStream in(&stylesheetFile);
        QString stylesheet = in.readAll();
        this->setStyleSheet(stylesheet);
        stylesheetFile.close();
    }else
        qDebug() << "Ошибка открытия файла:" << stylesheetFile.fileName();




}

MainWindow::~MainWindow()
{
    delete ui;
}
