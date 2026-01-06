#pragma once

#include <QWidget>

namespace Ui {
class DrawioRenderWidget;
}
class StatusLamp;
class DrawioExporter;
class DrawIoExecutable;
class DrawioRenderWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DrawioRenderWidget(QWidget *parent = nullptr);
    ~DrawioRenderWidget();

    void setDrawioExec(DrawIoExecutable* executable);

protected slots:
    void on_pushButton_DrawioInputDialog_clicked();
    void on_pushButton_DrawioOutputDialog_clicked();
private slots:
    void on_pushButton_Export_clicked();

private:
    Ui::DrawioRenderWidget *ui;
    //StatusLamp* m_DrawioRenderStatus;
    DrawioExporter* m_drawioExporter;
};

