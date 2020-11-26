/***********************************************************************
* COMPANY :   IPCREATOR
* AUTHOR  :   Tuan TRUONG
*
* FILENAME :  transferparams.cpp
*
* DESCRIPTION :
*     TransferParams class. Manage the transfer parameters UI
*
* CHANGES :
*
* VERSION	DATE		WHO			DETAIL
* 2.0 		11/26/20 	TT      	Initial file creation
*
*/
#include "transferparams.h"
#include "ui_transferparams.h"

TransferParams::TransferParams(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TransferParams)
{
    ui->setupUi(this);
    this->setWindowTitle("Transfer Parameters");
}

TransferParams::~TransferParams()
{
    delete ui;
}

void TransferParams::on_ok_clicked()
{
    emit txrParams(ui->pckLen->value(), ui->nbPcks->value());
    this->close();
}
