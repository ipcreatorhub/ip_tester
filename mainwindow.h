/***********************************************************************
* COMPANY :   IPCREATOR
* AUTHOR  :   Tuan TRUONG
*
* FILENAME :  mainwindow.h
*
* DESCRIPTION :
*     MainWindow class. Manage the whole test bench and the main UI
*
* CHANGES :
*
* VERSION	DATE		WHO			DETAIL
* 2.0 		11/26/20 	TT      	Initial file creation
*
*/
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>
#include <QTimer>
#include "tcp_server.h"
#include "tcp_client.h"
#include "tcpstreamchecker.h"
#include "udp_server.h"
#include "transferparams.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    QThread TcpStreamCheckerThread;
    QThread TcpClientThread;
    QThread TcpClientStreamCheckerThread;
    QThread UdpServerThread;
    QThread UdpServerStreamCheckerThread;
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    /* Statistic information reformat */
    QString byteCounterReformat(quint64 nbBytes);
public slots:
    /* Log */
    void logMsg(const QString &msg);
    /* Timers */
    void updateStat();
    void updateSystemDateTime();

    /* Speed select */
    void speedSelectHandle(int idx);

    /* TCP server */
    void tcpServerOpenHandle();
    void tcpServerCloseHandle();
    void tcpServerConnectedHandle(bool state);
    void tcpServerReceivedHandle(const QByteArray &data);
    void tcpServerBytesSentHandle(qint64 nbBytes);
    void tcpServerSendHandle();
    void tcpServerSend(int len, int nbPcks);
    void sendToClient(int pckLen, int nbPck);
    void tcpCorrectReport(quint64 verifCount);
    void tcpErrorReport(quint64 errorCount);
    void tcpServerDataGenHandle(quint64 nbBytes);
    void setTcpPRBSCheck(bool checked);

    /* TCP client */
    void tcpClientOpenHandle();
    void tcpClientCloseHandle();
    void tcpClientConnectedHandle(bool state);
    void tcpClientReceivedHandle(const QByteArray &data);
    void tcpClientBytesSentHandle(qint64 nbBytes);
    void tcpClientSendHandle();
    void tcpClientSend(int len, int nbPcks);
    void sendToServer(int pckLen, int nbPck);
    void tcpClientCorrectReport(quint64 verifCount);
    void tcpClientErrorReport(quint64 errorCount);
    void tcpClientDataGenHandle(quint64 nbBytes);
    void setTcpClientPRBSCheck(bool checked);

    /* UDP Server */
    void udpServerReceivedHandle(const char *data, const int nbPck, const int pckLen);
    void udpServerPckReceivedHandle(const int len);
    void udpServerBytesSentHandle(qint64 nbBytes);
    void udpServerSendHandle();
    void udpServerSend(int len, int nbPcks);
    void udpServerBindHandle();
    void udpServerReceiveHandle();
    void udpServerbindResultHandle(bool ret);
    void sendDatagramsToHost(int pckLen, int nbPck);
    void udpServerCorrectReport(quint64 verifCount);
    void udpServerErrorReport(quint64 errorCount);
    void udpServerDataGenHandle(quint64 nbBytes);
    void setUdpServerPRBSCheck(bool checked);

signals:
    void tcpStreamCheckerRequest(const QByteArray &data);
    void tcpStreamCheckerReset();
    void sendToClientTestRequest(int pckLen, int nbPck);
    void sendToServerTestRequest(int pckLen, int nbPck);

    void tcpClientStreamCheckerRequest(const QByteArray &data);
    void tcpClientStreamCheckerReset();
    void tcpClientConnectToHostRequest(QString host, quint16 port);
    void tcpClientDisconnectFromHostRequest();

    void udpServerBindRequest(const QHostAddress &srcAddress, const quint16 &srcPort, const QHostAddress &dstAddress, const quint16 &dstPort);
    void udpStreamCheckerRequest(const char *data, const int nbPck, const int pckLen);
    void udpStreamCheckerReset();
    void sendDatagramToHostTestRequest(int pckLen, int nbPck);
    void udpServerReceiveRequest(int nbPck, int len);

private:
    Ui::MainWindow *ui;

    int nbLogLines;

    tcp_server *mTcpServer = nullptr;
    tcp_client *mTcpClient = nullptr;

    quint64 mTcpServerNbBytesReceived;
    quint64 mTcpServerNbBytesSent;
    quint64 mTcpCorrectCount;
    quint64 mTcpErrorCount;

    TcpStreamChecker *mTcpStreamChecker;
    bool mTcpPRBSIsChecked;

    quint64 mTcpClientNbBytesReceived;
    quint64 mTcpClientNbBytesSent;
    quint64 mTcpClientCorrectCount;
    quint64 mTcpClientErrorCount;

    TcpStreamChecker *mTcpClientStreamChecker;
    bool mTcpClientPRBSIsChecked;

    QTimer *mStatTimer;
    QTimer *mDateTimer;

    udp_server *mUdpServer;
    TcpStreamChecker *mUdpStreamChecker;

    quint64 mUdpServerNbBytesReceived;
    quint64 mUdpServerNbPcksReceived;
    quint64 mUdpServerNbBytesSent;
    quint64 mUdpCorrectCount;
    quint64 mUdpErrorCount;
    bool mUdpPRBSIsChecked;

    TransferParams *mTcpServerTransferParams;
    TransferParams *mTcpClientTransferParams;
    TransferParams *mUdpServerTransferParams;
};

#endif // MAINWINDOW_H
