/***********************************************************************
* COMPANY :   IPCREATOR
* AUTHOR  :   Tuan TRUONG
*
* FILENAME :  transferparams.h
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
#ifndef TRANSFERPARAMS_H
#define TRANSFERPARAMS_H

#include <QWidget>

namespace Ui {
class TransferParams;
}

class TransferParams : public QWidget
{
    Q_OBJECT

public:
    explicit TransferParams(QWidget *parent = nullptr);
    ~TransferParams();
signals:
    void txrParams(int pckLen, int nbPcks);
private slots:
    void on_ok_clicked();

private:
    Ui::TransferParams *ui;
};

#endif // TRANSFERPARAMS_H
