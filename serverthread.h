/***********************************************************************
* COMPANY :   IPCREATOR
* AUTHOR  :   Tuan TRUONG
*
* FILENAME :  serverthread.h
*
* DESCRIPTION :
*     ServerThread class. Manage the TCP server in a thread apart from the main thread
*
* CHANGES :
*
* VERSION	DATE		WHO			DETAIL
* 2.0 		11/26/20 	TT      	Initial file creation
*
*/
#ifndef SERVERTHREAD_H
#define SERVERTHREAD_H

#include <QObject>
#include <QThread>
#include <QTcpSocket>

class ServerThread : public QThread
{
    Q_OBJECT
public:
    explicit ServerThread(int socketDescriptor, QObject *parent);
    ~ServerThread();

    //void run() override;

signals:
    void error(QTcpSocket::SocketError socketError);
    void received(const QByteArray & data);
    void connected();
    void disconnected();
    void bytesSent(quint64 nbBytes);
public slots:
    void readAll();
    void connectedHandle();
    void disconnectedHandle();
    qint64 sendToClientBulk(const char *data, int pckLen, int nbPck);
    qint64 sendToClient(const char *data, int len);
    qint64 sendToClient(const QString &data);
    qint64 sendToClient(const QByteArray &data);
    void disconnectFromHostHandle();
    bool isConnected(){return mConnected;}
private:
    int socketDescriptor;
    QTcpSocket *tcpSocket;
    QString text;
    bool mConnected;
};

#endif // SERVERTHREAD_H
