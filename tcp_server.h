/***********************************************************************
* COMPANY :   IPCREATOR
* AUTHOR  :   Tuan TRUONG
*
* FILENAME :  tcp_server.h
*
* DESCRIPTION :
*     tcp_server class. Manage the TCP server used in the test bench. The server resides on a thread apart from the main UI thread
*
* CHANGES :
*
* VERSION	DATE		WHO			DETAIL
* 2.0 		11/26/20 	TT      	Initial file creation
*
*/
#ifndef TCP_SERVER_H
#define TCP_SERVER_H

//#include <QObject>
//#include <QDataStream>
//#include <QDialog>
//#include <QMessageBox>
#include <QTcpServer>

class tcp_server : public QTcpServer
{
    Q_OBJECT
public:
    tcp_server(QObject *parent = 0);
    ~tcp_server();

protected:
    void incomingConnection(qintptr socketDescriptor) override;
signals:
    void received(const QByteArray & data);
    void err_msg(const QString &msg);
    void connected(bool state);
    void disconnectFromHostReq();

    void sendToClientReq(const char *data, int len);
    void sendToClientReq(const QString &data);
    void sendToClientReq(const QByteArray &data);
    void sendToClientBulkReq(const char *data, int pckLen, int nbPck);

    void bytesSent(qint64 nbBytes);
private slots:

public slots:
    bool tcpOpen(QHostAddress address, quint16 port);
    void tcpClose();
    void disconnectFromHostHandle(){ emit  disconnectFromHostReq();}
    void newConnectionHandle();
    void disconnectHandle();
    void readAll(const QByteArray & data);
    void sendToClientDemo(const char *data, int pckLen, int nbPck);
    void sendToClient(const char *data, int len);
    void sendToClient(const QString &data);
    void sendToClient(const QByteArray &data);
    void bytesSentHandle(int nbBytes);
private:
    //QTcpServer *tcpServer = nullptr;
    //QTcpSocket *clientConnection = nullptr;
};

#endif // TCP_SERVER_H
