#include "DrawioRenderWidget.h"
#include "DrawioExporter.h"
#include "StatusLamp.h"
#include "qdebug.h"
#include "qfiledialog.h"
#include "ui_DrawioRenderWidget.h"


DrawioRenderWidget::DrawioRenderWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::DrawioRenderWidget)
{
    ui->setupUi(this);

    ui->drawioRenderStatus->setLampState(LampState::LampStateOff);
    ui->drawioRenderStatus->setText("Render Result");

    m_drawioExporter = new DrawioExporter(this);
}

DrawioRenderWidget::~DrawioRenderWidget()
{
    delete ui;
}

void DrawioRenderWidget::setDrawioExec(DrawIoExecutable *executable)
{
    m_drawioExporter->setDrawIoExec(executable);
}

void DrawioRenderWidget::on_pushButton_DrawioInputDialog_clicked()
{
    QFileDialog* fileDialog = new QFileDialog(this);
    fileDialog->setFileMode(QFileDialog::FileMode::ExistingFile);
    fileDialog->setAcceptMode(QFileDialog::AcceptMode::AcceptOpen);
    fileDialog->setViewMode(QFileDialog::ViewMode::Detail);
    fileDialog->setNameFilter(tr("draw.io(*.drawio);;All Files(*.*)"));

    QFileInfo fileinfo(ui->lineEdit_DrawioInput->text());
    fileDialog->setDirectory(fileinfo.dir());
    fileDialog->selectFile(fileinfo.absoluteFilePath());

    if(fileDialog->exec()){
        auto files = fileDialog->selectedFiles();
        if(files.size()){
            ui->lineEdit_DrawioInput->setText(files[0]);
        }
    }
}


void DrawioRenderWidget::on_pushButton_DrawioOutputDialog_clicked()
{
    QFileDialog* fileDialog = new QFileDialog(this);
    fileDialog->setFileMode(QFileDialog::FileMode::AnyFile);
    fileDialog->setAcceptMode(QFileDialog::AcceptMode::AcceptSave);
    fileDialog->setViewMode(QFileDialog::ViewMode::Detail);
    //fileDialog->setNameFilter(tr("jpeg image(*.jpg);;All Files(*.*)"));

    QFileInfo fileinfo(ui->lineEdit_DrawioOutput->text());
    fileDialog->setDirectory(fileinfo.dir());
    fileDialog->selectFile(fileinfo.absoluteFilePath());

    if(fileDialog->exec()){
        auto files = fileDialog->selectedFiles();
        if(files.size()){
            ui->lineEdit_DrawioOutput->setText(files[0]);
        }
    }
}


void DrawioRenderWidget::on_pushButton_Export_clicked()
{
    ui->drawioRenderStatus->setLampState(LampState::LampStateMedium);
    QCoreApplication::processEvents();

    m_drawioExporter->setScale(ui->lineEdit_DrawioRenderScale->text().toFloat());
    int ret = m_drawioExporter->exportToImage(
        ui->lineEdit_DrawioInput->text(),
        ui->lineEdit_DrawioOutput->text());

    if(ret)
    {
        ui->drawioRenderStatus->setLampState(LampState::LampStateOff);
    }
    else
    {
        ui->drawioRenderStatus->setLampState(LampState::LampStateOn);
    }
}

