/***********************************************************************
* COMPANY :   IPCREATOR
* AUTHOR  :   Tuan TRUONG
*
* FILENAME :  udp_server.h
*
* DESCRIPTION :
*     udp_server class. Manage UDP TX and RX
*
* CHANGES :
*
* VERSION	DATE		WHO			DETAIL
* 2.0 		11/26/20 	TT      	Initial file creation
*
*/
#ifndef UDP_SERVER_H
#define UDP_SERVER_H

#include <QObject>
#include <QUdpSocket>
#include <QEventLoop>
#include <QNetworkDatagram>
#include <QTimer>
#include <winsock2.h>
#include <errno.h>

class udp_server : public QObject
{
    Q_OBJECT
public:
    explicit udp_server(QObject *parent = nullptr, QHostAddress address = QHostAddress::AnyIPv4, quint16 port = 3110);
    ~udp_server();

signals:
    void receivedPck(const int len);
    void received(const char *data, const int nbPck, const int pckLen);
    void bytesWritten(qint64 nbBytes);
    void bindResult(bool stat);
    void message(const QString &msg);
public slots:
    bool bindSock(const QHostAddress &srcAddress, const quint16 &srcPort, const QHostAddress &dstAddress, const quint16 &dstPort);
    void bytesWrittenHandle(qint64 nbBytes);
    void readPendingDatagrams();
    void sendDatagram(const char *data, int len, QHostAddress host, quint16 port);
    void sendDatagramBulk(const char *data, int pckLen, int nbPck);
    void receiveHandle(int nbPck, int len);
    void setHostAddr(const QString &hostAddr){mHostAddress = hostAddr;}
    void setHostPort(int port){ mHostPort = port; }
    void timeOutHandle();
private:
    SOCKET mSockFd;
    struct sockaddr_in mServAddr;

    QString mHostAddress;
    quint16 mHostPort;
    //QEventLoop *mEventLoop;
    QTimer *mTimer;
    quint64 mOffset, mPckLen, mNbPck, mActualPck;

    char *mData;
    char *mReceiveBuf;
};

#endif // UDP_SERVER_H
