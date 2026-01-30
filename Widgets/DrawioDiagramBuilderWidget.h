#pragma once
#include <QWidget>
#include <QMap>
namespace Ui {
class DrawioDiagramBuilderWidget;
}
class DrawioExporter;
class QListWidgetItem;
class DrawIoExecutable;
class DrawioFileParser;
class SimplifiedFieldsParserWidget;
class DrawioDiagramBuilderWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DrawioDiagramBuilderWidget(QWidget *parent = nullptr);
    ~DrawioDiagramBuilderWidget();
    void setDrawioExec(DrawIoExecutable* executable);

    void addPlaceHolder(QString key, QStringList list);
    void removePlaceHolder(QString key);



private slots:
    void on_pushButton_DrawioTemplateDialog_clicked();

    void on_pushButton_DrawioOutputDialog_clicked();

    void on_pushButton_ImageOutputDialog_clicked();

private:
    Ui::DrawioDiagramBuilderWidget *ui;
    SimplifiedFieldsParserWidget* m_simpleTextParser;
    DrawioExporter* m_drawioExporter;
    DrawioFileParser* m_drawioFileParser;
    QMap<QString, QStringList> m_placeHolders;


    void onAddPlaceHolder();
    void onRemovePlaceHolder();
    void onPlaceHolderItemChanged(QListWidgetItem *item);

    void placeHolderSelectionChanged();



    void onAddPlaceHolderData();
    void onRemovePlaceHolderData();
    void onPlaceHolderDataItemChanged(QListWidgetItem *item);

    void onDrawioGenerateButton();
    void onImageGenerateButton();
};



