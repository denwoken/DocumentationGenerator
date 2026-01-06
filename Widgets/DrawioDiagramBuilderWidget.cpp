#include "DrawioDiagramBuilderWidget.h"
#include "DrawioExporter.h"
#include "qdebug.h"
#include "qtimer.h"
#include "ui_DrawioDiagramBuilderWidget.h"

DrawioDiagramBuilderWidget::DrawioDiagramBuilderWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::DrawioDiagramBuilderWidget)
{
    ui->setupUi(this);


    ui->drawioBuildStatus->setLampState(LampState::LampStateOff);
    ui->drawioBuildStatus->setText("Generation Result");



    ui->listWidget_placeHoldersNames->setEditTriggers(
        QAbstractItemView::DoubleClicked |
        QAbstractItemView::EditKeyPressed |
        QAbstractItemView::SelectedClicked
        );


    auto *lw = ui->listWidget_placeHoldersNames;
    for (int i = 0; i < lw->count(); ++i) {
        auto *item = lw->item(i);
        if (!item) continue;
        QString key = item->text().trimmed();
        addPlaceHolder(key, {});
    }

    /*
    addPlaceHolder("className", {"Name of the class"});
    addPlaceHolder("classFields", {
                                   "class Field 1",
                                   "class Field 2"});
    addPlaceHolder("publicMethods", {"public Method 1", "public Method 2"});
    addPlaceHolder("privateMethods", {"private Method 1", "private Method 2"});

    */

    addPlaceHolder("className", {"PlcModbusDevice"});

    addPlaceHolder("classFields", {
      "QModbusTcpClient *m_modbus = nullptr",
      "quint16 m_docVersion = 0",
      "quint16 m_complexID = 0",
      "int m_plcAmount = 0",
      "int m_plcCount = 0",
      "QString m_tcpIp = \"\"",
      "int m_tcpPort = -1",
      "quint8 m_serverAddres = 255",
      "quint16 m_holdingStartAddress = 0",
      "quint16 m_holdingAddressLenght = 10",
      "bool m_holdingPeriodicRequestEnable = false",
      "quint16 m_inputStartAddress = 0",
      "quint16 m_inputAddressLenght = 10",
      "bool m_inputPeriodicRequestEnable = false",
      "quint16 m_periodicRequestTime = 250",
      "int m_reboundTime = 20",
      "bool m_virtualInputRegisters = false",
      "bool m_heartbeat = true",
      "quint16 m_heartbeatTime = 1000",
      "quint16 m_heartbeatAddr = 112",
      "QTimer *m_autoConnectionTimer = nullptr",
      "QTimer *m_periodicRequestTimer = nullptr",
      "QTimer *m_heartbeatTimer = nullptr",
      "bool m_heartbeatValue = true",
      "bool m_modbusBusy = false",
      "QQueue<PlcQueueData> m_bufferComands",
      "bool m_readLoop = false",
      "QQueue<std::pair<quint16, quint16>> m_readLoopAddr",
      "QVector<quint16> m_readLoopResult",
      "const quint16 ReadMaxLen = 125",
      "const quint16 WriteMaxLen = 123"
    });


    addPlaceHolder("publicMethods", {
        "PlcModbusDevice(QObject *parent = nullptr)",
        "~PlcModbusDevice()",
        "bool readSettings(QString path) override",
        "bool writeSettings(QString path) override",
        "bool connectDevice() override",
        "bool disconnectDevice() override",
        "bool setAutoConnection(bool autoConnection) override",
        "bool setConnectionParameters(QString ipAddr, quint16 port)",
        "bool setAutoConnectionTime(quint32 time) override",
        "void setPeriodicRequestTime(quint16 periodicRequestTime)",
        "int getReboundTime()",
        "void setHearbeatEnable(bool enable)",
        "void setHeartbeatPeriod(quint16 periodicRequestTime)",
        "bool setHoldingRegister(quint16 address, quint16 value)",
        "bool setHoldingRegisters(quint16 address, QVector<quint16> values)",
        "bool getHoldingRegisters(quint16 address, quint16 lenght)",
        "bool getInputRegisters(quint16 address, quint16 lenght)",
        "bool getAllHoldingRegisters()",
        "bool getAllInputRegisters()",
        "int getBufferSize()"
    });

    addPlaceHolder("protectedMethods", {
       "void heartbeat()",
       "bool findDevice()",
       "void modbusStateChanged(QModbusDevice::State state)",
       "void queueManager()",
       "bool writeHoldingRegisters(quint16 address, QVector<quint16> data)",
       "void writeHoldingRegistersUpdate()",
       "bool readInputRegisters(quint16 address, quint16 lenght)",
       "void readInputRegistersUpdate()",
       "bool readHoldingRegisters(quint16 address, quint16 lenght)",
       "void readHoldingRegistersUpdate()",
       "void getAllRegs()",
       "void readAllHoldingRegisters()",
       "void readAllHoldingRegistersUpdate()",
       "void readAllInputRegisters()",
       "void readAllInputRegistersUpdate()",
       "bool checkConnectionParameters()",
       "bool check()"
    });

    addPlaceHolder("signals", {
      "void sigGetInputRegistersUpdate(quint16 startAddres, QVector<quint16> data)",
      "void sigGetHoldingRegistersUpdate(quint16 startAddres, QVector<quint16> data)",
      "void sigSetHoldingRegistersUpdate(quint16 startAddres, QVector<quint16> data)",
      "void sigGetAllHoldingRegistersUpdate(QVector<quint16> data)",
      "void sigGetAllInputRegistersUpdate(QVector<quint16> data)"
    });




    connect(ui->pushButton_addPlaceHolder, &QPushButton::clicked,
            this, &DrawioDiagramBuilderWidget::onAddPlaceHolder);

    connect(ui->pushButton_remPlaceHolder, &QPushButton::clicked,
            this, &DrawioDiagramBuilderWidget::onRemovePlaceHolder);

    connect(ui->listWidget_placeHoldersNames, &QListWidget::itemChanged,
            this, &DrawioDiagramBuilderWidget::onPlaceHolderItemChanged);


    connect(ui->listWidget_placeHoldersNames, &QListWidget::itemSelectionChanged,
            this, &DrawioDiagramBuilderWidget::placeHolderSelectionChanged);
    placeHolderSelectionChanged();










    m_drawioExporter = new DrawioExporter(this);
}

DrawioDiagramBuilderWidget::~DrawioDiagramBuilderWidget()
{
    delete ui;
}

void DrawioDiagramBuilderWidget::addPlaceHolder(QString key, QStringList list)
{
    auto *listWidget = ui->listWidget_placeHoldersNames;
    auto items = listWidget->findItems(key,Qt::MatchFlag::MatchCaseSensitive);
    if(items.size()){
        listWidget->setCurrentItem(items.at(0));
    }else
    {
        auto* item = new QListWidgetItem(key);
        item->setFlags(item->flags() | Qt::ItemIsEditable);
        item->setData(Qt::UserRole, item->text());
        listWidget->insertItem(listWidget->count()-1, item);
        listWidget->setCurrentItem(item);
    }


    auto iter = m_placeHolders.find(key);
    if(iter == m_placeHolders.end()){
        m_placeHolders.insert(key, list);
    }else{
        *iter = list;
    }
}

void DrawioDiagramBuilderWidget::removePlaceHolder(QString key)
{
    m_placeHolders.remove(key);

    auto *listWidget = ui->listWidget_placeHoldersNames;
    auto items = listWidget->findItems(key, Qt::MatchCaseSensitive);

    if (!items.isEmpty()) {
        int row = listWidget->row(items.first());
        delete listWidget->takeItem(row);
    }
}

void DrawioDiagramBuilderWidget::onAddPlaceHolder()
{
    auto selected = ui->listWidget_placeHoldersNames->selectedItems();

    int selectedRow = ui->listWidget_placeHoldersNames->count(); // по умолчанию в конец
    if (!selected.isEmpty())
        selectedRow = ui->listWidget_placeHoldersNames->row(selected.first()) + 1;


    auto iter = m_placeHolders.find("-");
    if(iter == m_placeHolders.end()){
        m_placeHolders.insert("-", {});

        auto* item = new QListWidgetItem("-");
        item->setFlags(item->flags() | Qt::ItemIsEditable);
        item->setData(Qt::UserRole, item->text());

        ui->listWidget_placeHoldersNames->insertItem(selectedRow, item);
        ui->listWidget_placeHoldersNames->setCurrentItem(item);
        ui->listWidget_placeHoldersNames->editItem(item);
    } else {
        auto list = ui->listWidget_placeHoldersNames->findItems("-",Qt::MatchFlag::MatchCaseSensitive);
        if(list.size()){
            ui->listWidget_placeHoldersNames->setCurrentItem(list.at(0));
        }else
        {
            auto* item = new QListWidgetItem("-");
            item->setFlags(item->flags() | Qt::ItemIsEditable);
            item->setData(Qt::UserRole, item->text());

            ui->listWidget_placeHoldersNames->insertItem(selectedRow, item);
            ui->listWidget_placeHoldersNames->setCurrentRow(selectedRow);
        }
    }
}

void DrawioDiagramBuilderWidget::onRemovePlaceHolder()
{
    auto selected = ui->listWidget_placeHoldersNames->selectedItems();
    if(selected.size()>0){
        auto selection = selected.at(0);
        int selectedRow = ui->listWidget_placeHoldersNames->row(selection);
        int rowCount = ui->listWidget_placeHoldersNames->count();

        ui->listWidget_placeHoldersNames->clearSelection();
        if(rowCount-1 > selectedRow) // если не послдний жлемент
        {
            ui->listWidget_placeHoldersNames->setCurrentRow(selectedRow+1);
        } else if(selectedRow > 0)
        {
            ui->listWidget_placeHoldersNames->setCurrentRow(selectedRow-1);
        }

        m_placeHolders.remove(selection->text());
        delete ui->listWidget_placeHoldersNames->takeItem(selectedRow);
    }
}

void DrawioDiagramBuilderWidget::onPlaceHolderItemChanged(QListWidgetItem *item)
{
    //qDebug() << "onPlaceHolderItemChanged";

    const QString oldKey = item->data(Qt::UserRole).toString();
    const QString newKey = item->text().trimmed();

    //  пустое имя запрещаем
    if (newKey.isEmpty()) {
        item->setText(oldKey);
        return;
    }

    //  если не поменялось — ничего не делаем
    if (newKey == oldKey) return;

    //  запрет дубликатов
    if (m_placeHolders.contains(newKey)) {
        item->setText(oldKey);
        return;
    }

    //  перенос значения в map (rename key)
    auto iter = m_placeHolders.find(oldKey);
    if (iter != m_placeHolders.end()) {
        auto value = iter.value();
        m_placeHolders.erase(iter);
        m_placeHolders.insert(newKey, value);
    } else {
        // если вдруг элемента не было в map
        m_placeHolders.insert(newKey, {});
    }

    //  обновляем "старое имя" в item
    item->setData(Qt::UserRole, newKey);

    //qDebug() << m_placeHolders;
}

void DrawioDiagramBuilderWidget::placeHolderSelectionChanged()
{
    //qDebug() << "placeHolderSelectionChanged";

    auto items = ui->listWidget_placeHoldersNames->selectedItems();
    if(items.size()){
        QString key = items.at(0)->text();
        ui->listWidget_placeHoldersData->clear();
        ui->listWidget_placeHoldersData->addItems(m_placeHolders[key]);
    }
}





















