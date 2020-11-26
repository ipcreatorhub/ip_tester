/***********************************************************************
* COMPANY :   IPCREATOR
* AUTHOR  :   Tuan TRUONG
*
* FILENAME :  mainwindow.cpp
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
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QDateTime>


#define     GIGA    ( 1000000000 )
#define     MEGA    ( 1000000 )
#define     KILO    ( 1000 )

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    mTcpServerNbBytesReceived(0),
    mTcpServerNbBytesSent(0),
    mTcpCorrectCount(0),
    mTcpErrorCount(0),
    mTcpPRBSIsChecked(false),
    mTcpClientNbBytesReceived(0),
    mTcpClientNbBytesSent(0),
    mTcpClientCorrectCount(0),
    mTcpClientErrorCount(0),
    mTcpClientPRBSIsChecked(false),
    mUdpServerNbBytesReceived(0),
    mUdpServerNbPcksReceived(0),
    mUdpServerNbBytesSent(0),
    mUdpCorrectCount(0),
    mUdpErrorCount(0),
    mUdpPRBSIsChecked(false),
    nbLogLines(0)
{
    ui->setupUi(this);

    this->setWindowTitle("FPGA IP Stack Test Bench");

    /* Speed select */
    QStringList speedSelList;
    speedSelList   << "1G Stack" << "10G Stack";
    ui->speedSelect->addItems(speedSelList);

    connect(ui->speedSelect, SIGNAL(currentIndexChanged(int)), this, SLOT(speedSelectHandle(int)));

    /* TCP server */
    mTcpServer = new tcp_server(this);

    connect(mTcpServer, &tcp_server::connected,
                  this, &MainWindow::tcpServerConnectedHandle);
    connect(mTcpServer, &tcp_server::received,
                  this, &MainWindow::tcpServerReceivedHandle);
    connect(mTcpServer, &tcp_server::bytesSent,
                  this, &MainWindow::tcpServerBytesSentHandle);

    connect(ui->serverOpen, &QPushButton::clicked,
            this, &MainWindow::tcpServerOpenHandle);
    connect(ui->serverClose, &QPushButton::clicked,
            this, &MainWindow::tcpServerCloseHandle);
    connect(ui->sendToClient, &QPushButton::clicked,
            this, &MainWindow::tcpServerSendHandle);
    /* TCP server transfer parameters */
    mTcpServerTransferParams = new TransferParams();
    connect(mTcpServerTransferParams, &TransferParams::txrParams,
                                this, &MainWindow::tcpServerSend);

    /* TCP client */
    mTcpClient = new tcp_client();

    connect(mTcpClient, &tcp_client::connected,
                  this, &MainWindow::tcpClientConnectedHandle);
    connect(mTcpClient, &tcp_client::received,
                  this, &MainWindow::tcpClientReceivedHandle);
    connect(mTcpClient, &tcp_client::bytesSent,
                  this, &MainWindow::tcpClientBytesSentHandle);
    connect(this, &MainWindow::tcpClientConnectToHostRequest,
                  mTcpClient, &tcp_client::connectToHost);
    connect(this, &MainWindow::tcpClientDisconnectFromHostRequest,
                  mTcpClient, &tcp_client::disconnectFromHost);

//    mTcpClient->moveToThread(&TcpClientThread);
//    TcpClientThread.start();

    connect(ui->connectToServer, &QPushButton::clicked,
            this, &MainWindow::tcpClientOpenHandle);
    connect(ui->disconnectFromServer, &QPushButton::clicked,
            this, &MainWindow::tcpClientCloseHandle);
    connect(ui->sendToServer, &QPushButton::clicked,
            this, &MainWindow::tcpClientSendHandle);

    /* TCP client transfer parameters */
    mTcpClientTransferParams = new TransferParams();
    connect(mTcpClientTransferParams, &TransferParams::txrParams,
                                this, &MainWindow::tcpClientSend);

    /* TCP server Stream checker */
    mTcpStreamChecker = new TcpStreamChecker();

    mTcpStreamChecker->moveToThread(&TcpStreamCheckerThread);

    connect(this, &MainWindow::tcpStreamCheckerRequest, mTcpStreamChecker, &TcpStreamChecker::streamCheck);
    connect(this, &MainWindow::tcpStreamCheckerReset, mTcpStreamChecker, &TcpStreamChecker::PRBSGenReset);
    connect(this, &MainWindow::sendToClientTestRequest, mTcpStreamChecker, &TcpStreamChecker::sendToHostTest);
    connect(mTcpStreamChecker, &TcpStreamChecker::errorCount, this, &MainWindow::tcpErrorReport);
    connect(mTcpStreamChecker, &TcpStreamChecker::verifCount, this, &MainWindow::tcpCorrectReport);
    connect(mTcpStreamChecker, &TcpStreamChecker::DataGenerating, this, &MainWindow::tcpServerDataGenHandle);
    connect(mTcpStreamChecker, &TcpStreamChecker::sendToHostReq, mTcpServer, &tcp_server::sendToClientDemo);

    TcpStreamCheckerThread.start();

    /* TCP client Stream checker */
    mTcpClientStreamChecker = new TcpStreamChecker();

    connect(this, &MainWindow::tcpClientStreamCheckerRequest, mTcpClientStreamChecker, &TcpStreamChecker::streamCheck);
    connect(this, &MainWindow::tcpClientStreamCheckerReset, mTcpClientStreamChecker, &TcpStreamChecker::PRBSGenReset);    
    connect(this, &MainWindow::sendToServerTestRequest, mTcpClientStreamChecker, &TcpStreamChecker::sendToHostTest);
    connect(mTcpClientStreamChecker, &TcpStreamChecker::errorCount, this, &MainWindow::tcpClientErrorReport);
    connect(mTcpClientStreamChecker, &TcpStreamChecker::verifCount, this, &MainWindow::tcpClientCorrectReport);
    connect(mTcpClientStreamChecker, &TcpStreamChecker::DataGenerating, this, &MainWindow::tcpClientDataGenHandle);
    connect(mTcpClientStreamChecker, &TcpStreamChecker::sendToHostReq, mTcpClient, &tcp_client::sendToHostDemo);

    mTcpClientStreamChecker->moveToThread(&TcpClientStreamCheckerThread);
    TcpClientStreamCheckerThread.start();

    mTcpClient->moveToThread(&TcpClientThread);
    TcpClientThread.start();

    /* UDP Server */
    mUdpServer = new udp_server();

    qRegisterMetaType<QHostAddress>("QHostAddress");

    connect(mUdpServer, &udp_server::received,
                  this, &MainWindow::udpServerReceivedHandle);
    connect(mUdpServer, &udp_server::receivedPck,
                  this, &MainWindow::udpServerPckReceivedHandle);
    connect(mUdpServer, &udp_server::bytesWritten,
                  this, &MainWindow::udpServerBytesSentHandle);
    connect(this, &MainWindow::udpServerBindRequest,
                  mUdpServer, &udp_server::bindSock);
    connect(this, &MainWindow::udpServerReceiveRequest,
                  mUdpServer, &udp_server::receiveHandle);
    connect(mUdpServer, &udp_server::bindResult,
                  this, &MainWindow::udpServerbindResultHandle);
    connect(mUdpServer, &udp_server::message,
                  this, &MainWindow::logMsg);
    mUdpServer->setHostAddr(ui->udpDstIpAddr->text());
    mUdpServer->setHostPort(ui->udpDstPort->value());
    connect(ui->udpDstIpAddr, &QLineEdit::textChanged, mUdpServer, &udp_server::setHostAddr);
    connect(ui->udpDstPort, SIGNAL(valueChanged(int)), mUdpServer, SLOT(setHostPort(int)));

    mUdpServer->moveToThread(&UdpServerThread);
    UdpServerThread.start();

    connect(ui->sendDatagram, &QPushButton::clicked,
            this, &MainWindow::udpServerSendHandle);
    connect(ui->udpBind, &QPushButton::clicked,
            this, &MainWindow::udpServerBindHandle);
    connect(ui->rcvDatagram, &QPushButton::clicked,
            this, &MainWindow::udpServerReceiveHandle);

    /* TCP client transfer parameters */
    mUdpServerTransferParams = new TransferParams();
    connect(mUdpServerTransferParams, &TransferParams::txrParams,
                                this, &MainWindow::udpServerSend);

    /* UDP Server stream checker */
    mUdpStreamChecker = new TcpStreamChecker();

    connect(this, &MainWindow::udpStreamCheckerRequest, mUdpStreamChecker, &TcpStreamChecker::UDPstreamCheck);
    connect(this, &MainWindow::udpStreamCheckerReset, mUdpStreamChecker, &TcpStreamChecker::PRBSGenReset);
    connect(this, &MainWindow::sendDatagramToHostTestRequest, mUdpStreamChecker, &TcpStreamChecker::sendToHostTest);
    connect(mUdpStreamChecker, &TcpStreamChecker::errorCount, this, &MainWindow::udpServerErrorReport);
    connect(mUdpStreamChecker, &TcpStreamChecker::verifCount, this, &MainWindow::udpServerCorrectReport);
    connect(mUdpStreamChecker, &TcpStreamChecker::DataGenerating, this, &MainWindow::udpServerDataGenHandle);
    connect(mUdpStreamChecker, &TcpStreamChecker::sendToHostReq, mUdpServer, &udp_server::sendDatagramBulk);
    connect(mUdpStreamChecker, &TcpStreamChecker::message, this, &MainWindow::logMsg);

    mUdpStreamChecker->moveToThread(&UdpServerStreamCheckerThread);


    UdpServerStreamCheckerThread.start();

    connect(ui->tcpServerPRBSVerif, &QCheckBox::toggled, this, &MainWindow::setTcpPRBSCheck);
    connect(ui->tcpClientPRBSVerif, &QCheckBox::toggled, this, &MainWindow::setTcpClientPRBSCheck);
    connect(ui->udpPRBSVerif, &QCheckBox::toggled, this, &MainWindow::setUdpServerPRBSCheck);


    /* Timers */
    mStatTimer = new QTimer(this);
    mStatTimer->setSingleShot(false);
    mStatTimer->setInterval(500);
    connect(mStatTimer, &QTimer::timeout, this, &MainWindow::updateStat);
    mStatTimer->start();

    mDateTimer = new QTimer(this);
    mDateTimer->setSingleShot(false);
    mDateTimer->setInterval(1000);
    connect(mDateTimer, &QTimer::timeout, this, &MainWindow::updateSystemDateTime);
    mDateTimer->start();

}

MainWindow::~MainWindow()
{
    delete ui;

    delete mTcpServer;
    delete mTcpClient;

    TcpStreamCheckerThread.quit();

    TcpStreamCheckerThread.wait();

    TcpClientThread.quit();
    TcpClientThread.wait();

    TcpClientStreamCheckerThread.quit();

    TcpClientStreamCheckerThread.wait();

    UdpServerThread.quit();
    UdpServerThread.wait();

    UdpServerStreamCheckerThread.quit();
    UdpServerStreamCheckerThread.wait();

    delete mStatTimer;
    delete mDateTimer;

    delete mTcpServerTransferParams;
    delete mTcpClientTransferParams;
    delete mUdpServerTransferParams;
}

/*!
 * \brief byteCounterReformat. Reformat the number of bytes for user-friendly look
 * \param nbBytes is the number of bytes
 * \return
 */
QString MainWindow::byteCounterReformat(quint64 nbBytes)
{
    double sample;
    QString format;
    if(nbBytes > GIGA)
    {
        sample = (double)nbBytes / (double)GIGA;
        format = QString::number(sample,'f', 3) + " GB";
    }
    else if (nbBytes > MEGA)
    {
        sample = (double)nbBytes / (double)MEGA;
        format = QString::number(sample,'f', 3) + " MB";
    }
    else if (nbBytes > KILO)
    {
        sample = (double)nbBytes / (double)KILO;
        format = QString::number(sample,'f', 0) + " KB";
    }
    else
    {
        sample = (double)nbBytes;
        format = QString::number(sample,'f', 0) + " B";
    }

    return format;
}

void MainWindow::logMsg(const QString &msg)
{
    if(nbLogLines > 1000)
    {
        nbLogLines = 0;
        ui->systemLog->clear();
    }
    ui->systemLog->appendPlainText(msg);
    nbLogLines++;
}

/*!
 * \brief MainWindow::updateStat. Update statistic information display
 */
void MainWindow::updateStat()
{
    if(ui->tcpServerStat->isChecked())
    {
        ui->tcpServerReceptionLabel->setText(byteCounterReformat(mTcpServerNbBytesReceived));
        ui->tcpServerPRBSLabelCorrect->setText(byteCounterReformat(mTcpCorrectCount));
        ui->tcpServerPRBSLabelError->setText(byteCounterReformat(mTcpErrorCount));
        ui->tcpServerTransmissionLabel->setText(byteCounterReformat(mTcpServerNbBytesSent));
    }

    if(ui->tcpClientStat->isChecked())
    {
        ui->tcpClientReceptionLabel->setText(byteCounterReformat(mTcpClientNbBytesReceived));
        ui->tcpClientPRBSLabelCorrect->setText(byteCounterReformat(mTcpClientCorrectCount));
        ui->tcpClientPRBSLabelError->setText(byteCounterReformat(mTcpClientErrorCount));
        ui->tcpClientTransmissionLabel->setText(byteCounterReformat(mTcpClientNbBytesSent));
    }

    if(ui->udpStat->isChecked())
    {
        ui->udpReceptionLabel->setText(QString::number(mUdpServerNbPcksReceived));
        ui->udpPRBSLabelCorrect->setText(byteCounterReformat(mUdpCorrectCount));
        ui->udpPRBSLabelError->setText(byteCounterReformat(mUdpErrorCount));
        ui->udpTransmissionLabel->setText(byteCounterReformat(mUdpServerNbBytesSent));
    }

    return;
}

/*!
 * \brief MainWindow::updateSystemDateTime. Update system date and system time display
 */
void MainWindow::updateSystemDateTime()
{
    QTime time = QTime::currentTime();
    QDate date = QDate::currentDate();

    ui->date->setText(date.toString());
    ui->time->setText(time.toString());

    return;
}

/*!
 * \brief MainWindow::speedSelectHandle. Update the speed mode when the combobox index is changed
 * \param idx is the idx emitted from the combobox
 */
void MainWindow::speedSelectHandle(int idx)
{
    switch (idx) {
    case 0:
        mTcpStreamChecker->setSpeedMode(TcpStreamChecker::sm1G);
        mTcpClientStreamChecker->setSpeedMode(TcpStreamChecker::sm1G);
        mUdpStreamChecker->setSpeedMode(TcpStreamChecker::sm1G);
        break;
    case 1:
        mTcpStreamChecker->setSpeedMode(TcpStreamChecker::sm10G);
        mTcpClientStreamChecker->setSpeedMode(TcpStreamChecker::sm10G);
        mUdpStreamChecker->setSpeedMode(TcpStreamChecker::sm10G);
        break;
    default:
        mTcpStreamChecker->setSpeedMode(TcpStreamChecker::sm1G);
        mTcpClientStreamChecker->setSpeedMode(TcpStreamChecker::sm1G);
        mUdpStreamChecker->setSpeedMode(TcpStreamChecker::sm1G);
        break;
    }
    return;
}

/*!
 * \brief MainWindow::tcpServerOpenHandle. Open a TCP server for new connections
 */
void MainWindow::tcpServerOpenHandle()
{
    QHostAddress address(ui->serverIpAddr->text());
    int port = ui->serverPort->value();

    if(mTcpServer->tcpOpen(address, port))
    {
        ui->serverStatusLabel->setText("Server is listening.");
    }
    else
    {
       ui->serverStatusLabel->setText("Fail to open the server.");
    }

    emit tcpStreamCheckerReset();

    return;
}

/*!
 * \brief MainWindow::tcpServerCloseHandle. Close the TCP server. This will no longer listen for new connections.
 */
void MainWindow::tcpServerCloseHandle()
{
    mTcpServer->tcpClose();

    ui->serverStatusLabel->setText("Closed.");

    mTcpServerNbBytesReceived = 0;
    mTcpCorrectCount = 0;
    mTcpErrorCount = 0;

    /* Change the tcp server port for the next connection because the old port is hold by the system and only released some times later.
       In This version we have to wait before being able to reuse the same port
    */
    int port = ui->serverPort->value()+1;
    if(port > 65535)
    {
        port = 1024;
    }
    ui->serverPort->setValue(port);
    emit tcpStreamCheckerReset();

    return;
}

/*!
 * \brief MainWindow::tcpServerConnectedHandle. Handling connection event of the TCP server.
 * \param state
 */
void MainWindow::tcpServerConnectedHandle(bool state)
{
    if(state == true)
    {
        ui->serverStatusLabel->setText("Connected.");
    }
    else
    {
        ui->serverStatusLabel->setText("Disconnected.");
    }

    return;
}

/*!
 * \brief MainWindow::tcpServerReceivedHandle. Handling TCP receive event. Report the number of bytes received, check PRBS sequence if required.
 * \param data
 */
void MainWindow::tcpServerReceivedHandle(const QByteArray &data)
{
    int len = data.length();

    mTcpServerNbBytesReceived += len;

    if(mTcpPRBSIsChecked)
    {
        emit tcpStreamCheckerRequest(data);
    }

    return;
}

/*!
 * \brief MainWindow::tcpServerBytesSentHandle. Increase the number of byte sent for reporting.
 * \param nbBytes
 */
void MainWindow::tcpServerBytesSentHandle(qint64 nbBytes)
{
    mTcpServerNbBytesSent += nbBytes;

    return;
}

/*!
 * \brief MainWindow::tcpClientBytesSentHandle. Increase the number of byte sent for reporting.
 * \param nbBytes
 */
void MainWindow::tcpClientBytesSentHandle(qint64 nbBytes)
{
    mTcpClientNbBytesSent += nbBytes;

    return;
}

void MainWindow::tcpServerSendHandle()
{
    mTcpServerTransferParams->show();

    return;
}

void MainWindow::tcpServerSend(int len, int nbPcks)
{
    mTcpServerNbBytesSent = 0;
    logMsg(tr("Sending %1 segments of size %2 bytes.").arg(nbPcks).arg(len));
    sendToClient(len, nbPcks);

    return;
}

void MainWindow::sendToClient(int pckLen, int nbPck)
{
    emit sendToClientTestRequest(pckLen, nbPck);

    return;
}

/*!
 * \brief MainWindow::tcpCorrectReport. Report the number of bytes that have been verified by the tcp stream checker
 * \param verifCount is the number of bytes verified.
 */
void MainWindow::tcpCorrectReport(quint64 verifCount)
{
    mTcpCorrectCount = verifCount;

    return;
}
/*!
 * \brief MainWindow::tcpErrorReport. Report for error (if any) of the received TCP PRBS stream. This is a fatal error because the TCP stream should never contain error!
 * \param errorCount is the number of error reported by the tcp stream checker.
 */
void MainWindow::tcpErrorReport(quint64 errorCount)
{
    mTcpErrorCount = errorCount;

    return;
}

/*!
 * \brief MainWindow::tcpServerDataGenHandle. Handle data generating event emitted by the stream checker.
 * \param nbBytes
 */
void MainWindow::tcpServerDataGenHandle(quint64 nbBytes)
{
    ui->tcpServerTransmissionLabel->setText(byteCounterReformat(nbBytes) + " generated.");
}

/*!
 * \brief MainWindow::tcpClientDataGenHandle. Handle data generating event emitted by the stream checker.
 * \param nbBytes
 */
void MainWindow::tcpClientDataGenHandle(quint64 nbBytes)
{
    ui->tcpClientTransmissionLabel->setText(byteCounterReformat(nbBytes) + " generated.");
}

/*!
 * \brief MainWindow::setTcpPRBSCheck. Change the PRBS verification option.
 * \param checked is the verification status.
 */
void MainWindow::setTcpPRBSCheck(bool checked)
{
    mTcpPRBSIsChecked = checked;

    emit tcpStreamCheckerReset();

    mTcpServerNbBytesReceived = 0;
    mTcpServerNbBytesSent = 0;

    ui->tcpServerReceptionLabel->setText(byteCounterReformat(mTcpServerNbBytesReceived));
    ui->tcpServerTransmissionLabel->setText(byteCounterReformat(mTcpServerNbBytesSent));
    tcpErrorReport(0);
    tcpCorrectReport(0);

    return;
}

/*!
 * \brief MainWindow::tcpClientOpenHandle. Connect to server
 */
void MainWindow::tcpClientOpenHandle()
{
    QString address = ui->serverIpAddr_2->text();
    int port = ui->serverPort_2->value();

    emit tcpClientConnectToHostRequest(address, port);

    emit tcpClientStreamCheckerReset();

    return;
}

/*!
 * \brief MainWindow::tcpClientCloseHandle. Disconnect from the server.
 */
void MainWindow::tcpClientCloseHandle()
{
    emit tcpClientDisconnectFromHostRequest();

    emit tcpClientStreamCheckerReset();

    return;
}

/*!
 * \brief MainWindow::tcpClientConnectedHandle. Handle TCP client connection status.
 * \param state is the connection establishment state.
 */
void MainWindow::tcpClientConnectedHandle(bool state)
{
    if(state == true)
    {
        ui->clientStatusLabel->setText("Connected.");
    }
    else
    {
        ui->clientStatusLabel->setText("Disconnected.");
    }

    return;
}

/*!
 * \brief MainWindow::tcpClientReceivedHandle. Handling TCP receive event. Report the number of bytes received, check PRBS sequence if required.
 * \param data
 */
void MainWindow::tcpClientReceivedHandle(const QByteArray &data)
{
    int len = data.length();

    mTcpClientNbBytesReceived += len;

    if(mTcpClientPRBSIsChecked)
    {
        emit tcpClientStreamCheckerRequest(data);
    }

    return;
}

void MainWindow::tcpClientSendHandle()
{
    mTcpClientTransferParams->show();

    return;
}

void MainWindow::tcpClientSend(int len, int nbPcks)
{
    mTcpClientNbBytesSent = 0;
    logMsg(tr("Sending %1 segments of size %2 bytes.").arg(nbPcks).arg(len));
    sendToServer(len, nbPcks);

    return;
}

void MainWindow::sendToServer(int pckLen, int nbPck)
{
    emit sendToServerTestRequest(pckLen, nbPck);

    return;
}

/*!
 * \brief MainWindow::tcpClientCorrectReport. Report the number of bytes that have been verified by the tcp stream checker
 * \param verifCount is the number of bytes verified.
 */
void MainWindow::tcpClientCorrectReport(quint64 verifCount)
{
    mTcpClientCorrectCount = verifCount;

    return;
}
/*!
 * \brief MainWindow::tcpClientErrorReport. Report for error (if any) of the received TCP PRBS stream. This is a fatal error because the TCP stream should never contain error!
 * \param errorCount is the number of error reported by the tcp stream checker.
 */
void MainWindow::tcpClientErrorReport(quint64 errorCount)
{
    mTcpClientErrorCount = errorCount;

    return;
}

/*!
 * \brief MainWindow::setTcpClientPRBSCheck. Change the PRBS verification option.
 * \param checked is the verification status.
 */
void MainWindow::setTcpClientPRBSCheck(bool checked)
{
    mTcpClientPRBSIsChecked = checked;

    emit tcpClientStreamCheckerReset();

    mTcpClientNbBytesReceived = 0;
    mTcpClientNbBytesSent = 0;

    ui->tcpClientReceptionLabel->setText(byteCounterReformat(mTcpClientNbBytesReceived));
    ui->tcpClientTransmissionLabel->setText(byteCounterReformat(mTcpClientNbBytesSent));
    tcpClientErrorReport(0);
    tcpClientCorrectReport(0);

    return;
}

void MainWindow::udpServerReceivedHandle(const char *data, const int nbPck, const int pckLen)
{

    if(mUdpPRBSIsChecked)
    {
        logMsg("All datagrams have been received. Start checking.");
        emit udpStreamCheckerRequest(data, nbPck, pckLen);
    }

    return;
}

void MainWindow::udpServerPckReceivedHandle(const int len)
{
    mUdpServerNbBytesReceived += len;
    mUdpServerNbPcksReceived += 1;
}
void MainWindow::udpServerBytesSentHandle(qint64 nbBytes)
{
    mUdpServerNbBytesSent += nbBytes;

    return;
}
void MainWindow::udpServerSendHandle()
{
    mUdpServerTransferParams->show();

    return;
}

void MainWindow::udpServerSend(int len, int nbPcks)
{
    mUdpServerNbBytesSent = 0;
    logMsg(tr("Sending %1 datagrams of size %2 bytes.").arg(nbPcks).arg(len));
    sendDatagramsToHost(len, nbPcks);

    return;
}

void MainWindow::udpServerBindHandle()
{

    emit udpServerBindRequest(QHostAddress(ui->udpSrcIpAddr->text()), ui->udpSrcPort->value(), QHostAddress(ui->udpDstIpAddr->text()), ui->udpDstPort->value());

    return;
}
void MainWindow::udpServerReceiveHandle()
{
    logMsg("Waiting for 100'000 datagrams.");
    emit udpServerReceiveRequest(100000, 1024);
}
void  MainWindow::udpServerbindResultHandle(bool ret)
{
    if(ret)
    {
        ui->udpBindStat->setText("Binding success.");
    }
    else
    {
        ui->udpBindStat->setText("Binding failed.");
    }
}
void MainWindow::sendDatagramsToHost(int pckLen, int nbPck)
{
    emit sendDatagramToHostTestRequest(pckLen, nbPck);

    return;
}
void MainWindow::udpServerCorrectReport(quint64 verifCount)
{
    mUdpCorrectCount = verifCount;

    return;
}
void MainWindow::udpServerErrorReport(quint64 errorCount)
{
    mUdpErrorCount = errorCount;

    return;
}
void MainWindow::udpServerDataGenHandle(quint64 nbBytes)
{
    ui->udpTransmissionLabel->setText(byteCounterReformat(nbBytes) + " generated.");

    return;
}

void MainWindow::setUdpServerPRBSCheck(bool checked)
{
    mUdpPRBSIsChecked = checked;

    mUdpServerNbBytesReceived = 0;
    mUdpServerNbPcksReceived = 0;
    mUdpServerNbBytesSent = 0;
    mUdpCorrectCount = 0;
    mUdpErrorCount = 0;
    ui->udpReceptionLabel->setText(QString::number(mUdpServerNbPcksReceived));
    ui->udpTransmissionLabel->setText(byteCounterReformat(mUdpServerNbBytesSent));

    emit udpStreamCheckerReset();


    return;
}
