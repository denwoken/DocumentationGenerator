#pragma once
#include <QWidget>
#include <QMap>
namespace Ui {
class DrawioDiagramBuilderWidget;
}
class DrawioExporter;
class QListWidgetItem;
class DrawioDiagramBuilderWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DrawioDiagramBuilderWidget(QWidget *parent = nullptr);
    ~DrawioDiagramBuilderWidget();


    void addPlaceHolder(QString key, QStringList list);
    void removePlaceHolder(QString key);

private:
    Ui::DrawioDiagramBuilderWidget *ui;
    DrawioExporter* m_drawioExporter;

    QMap<QString, QStringList> m_placeHolders;


    void onAddPlaceHolder();
    void onRemovePlaceHolder();
    void onPlaceHolderItemChanged(QListWidgetItem *item);

    void placeHolderSelectionChanged();
};



