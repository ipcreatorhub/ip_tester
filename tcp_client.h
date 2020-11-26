/***********************************************************************
* COMPANY :   IPCREATOR
* AUTHOR  :   Tuan TRUONG
*
* FILENAME :  tcp_client.h
*
* DESCRIPTION :
*     tcp_client class. Manage the TCP client used in the test bench. The client resides on a thread apart from the main UI thread.
*
* CHANGES :
*
* VERSION	DATE		WHO			DETAIL
* 2.0 		11/26/20 	TT      	Initial file creation
*
*/
#ifndef TCP_CLIENT_H
#define TCP_CLIENT_H

#include <QObject>
#include <QDataStream>
#include <QDialog>
#include <QMessageBox>
#include <QHostAddress>
#include <QTcpSocket>

class tcp_client : public QObject
{
    Q_OBJECT
public:
    explicit tcp_client(QObject *parent = nullptr);
    ~tcp_client();

signals:
    void received(const QByteArray & rcv_data);
    void error_msg(const QString & msg);
    void connected(bool state);
    void bytesSent(quint64 nbBytes);
private slots:
    void displayError(QAbstractSocket::SocketError socketError);
    void tcpRead();
    void connectedHandle();
    void disconnectedHandle();
public slots:
    bool connectToHost(QString hostName, quint16 port);
    void disconnectFromHost();
    void abort();
    void sendToHost(const char *data, int len);
    void sendToHostDemo(const char *data, int pckLen, int nbPck);
private:
    QTcpSocket *tcpSocket = nullptr;
};

#endif // TCP_CLIENT_H
