#include "DialogHelp.h"
#include "ui_DialogHelp.h"

DialogHelp::DialogHelp(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DialogHelp)
{
    ui->setupUi(this);
}

DialogHelp::~DialogHelp()
{
    delete ui;
}
