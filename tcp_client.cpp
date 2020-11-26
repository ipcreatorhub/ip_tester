/***********************************************************************
* COMPANY :   IPCREATOR
* AUTHOR  :   Tuan TRUONG
*
* FILENAME :  tcp_client.cpp
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
#include "tcp_client.h"

tcp_client::tcp_client(QObject *parent) : QObject(parent)
{
    tcpSocket = new QTcpSocket(this);


    connect(tcpSocket, &QIODevice::readyRead, this, &tcp_client::tcpRead);
//    connect(tcpSocket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error),
//                this, &tcp_client::displayError);
    connect(tcpSocket, &QAbstractSocket::connected, this, &tcp_client::connectedHandle);
    connect(tcpSocket, &QAbstractSocket::disconnected, this, &tcp_client::disconnectedHandle);

}

tcp_client::~tcp_client()
{
    tcpSocket->abort();

    //delete tcpSocket;
}

/*!
 * \brief tcp_client::connectToHost
 * \param hostName is the ip address of the host
 * \param port is the host's port
 * \return true for success, false for failed or error
 */
bool tcp_client::connectToHost(QString hostName, quint16 port)
{
    bool ret = false;
    tcpSocket->abort();

    tcpSocket->connectToHost(QHostAddress(hostName), port);

    if(tcpSocket->waitForConnected(2000))
    {
        connectedHandle();
        qDebug() << "Connected.";
        ret = true;
    }
    else
    {
        tcpSocket->abort();
        qDebug() << "Cannot connect to host. Timeout occurs.";
        emit error_msg("Cannot connect to host. Timeout occurs.");
        disconnectedHandle();
        ret = false;
    }
    return ret;
}

/*!
 * \brief tcp_client::disconnectFromHost
 * Disconnect from host
 */
void tcp_client::disconnectFromHost()
{
    tcpSocket->disconnectFromHost();

    return;
}

/*!
 * \brief tcp_client::abort
 * Abort a connection
 */
void tcp_client::abort()
{
    tcpSocket->abort();

    return;
}

/*!
 * \brief tcp_client::connected
 * Connected handler. Retransmit the connection status
 */
void tcp_client::connectedHandle()
{
    emit error_msg("Connected");
    emit connected(true);
    return;
}

/*!
 * \brief tcp_client::disconnectedHandle. Retransmit disconnected signal.
 */
void tcp_client::disconnectedHandle()
{
    emit error_msg("Disconnected");
    emit connected(false);
    return;
}

/*!
 * \brief tcp_client::sendToHost
 * \param data contains data array that will be sent to the host
 * \param len is the data array length
 */
void tcp_client::sendToHost(const char *data, int len)
{
    qint64 ret = tcpSocket->write(data, len);
    emit bytesSent(ret);
    //tcpSocket->waitForBytesWritten();
    return;
}

/*!
 * \brief tcp_client::sendToHostDemo. This function is used essentially in IP Tester example design
 * \param data
 * \param pckLen
 * \param nbPck
 */
void tcp_client::sendToHostDemo(const char *data, int pckLen, int nbPck)
{
    int offset = 0;
    for(int i = 0; i < nbPck; i++)
    {
        sendToHost(data+offset, pckLen);
        offset += pckLen;
    }

    return;
}

/*!
 * \brief tcp_client::tcpRead
 * When there is something to read in the socket, this function will be invoked. The received data array will be broadcasted
 * via the signal void received(const QByteArray &msg)
 */
void tcp_client::tcpRead()
{
    emit received(tcpSocket->readAll());

    return;
}

/*!
 * \brief tcp_client::displayError
 * \param socketError
 * Handle socket error. Just display error message.
 */
void tcp_client::displayError(QAbstractSocket::SocketError socketError)
{
    switch (socketError) {
    case QAbstractSocket::RemoteHostClosedError:
        break;
    case QAbstractSocket::HostNotFoundError:
        emit error_msg("The host was not found. Please check the host name and port settings.");
//        QMessageBox::information(this, tr("TCP Client"),
//                                 tr("The host was not found. Please check the "
//                                    "host name and port settings."));
        break;
    case QAbstractSocket::ConnectionRefusedError:
        emit error_msg("The connection was refused by the peer. Make sure the fortune server is running, and check that the host name and port settings are correct.");
//        QMessageBox::information(this, tr("TCP Client"),
//                                 tr("The connection was refused by the peer. "
//                                    "Make sure the fortune server is running, "
//                                    "and check that the host name and port "
//                                    "settings are correct."));
        break;
    default:
        emit error_msg("The following error occurred: " + tcpSocket->errorString());
//        QMessageBox::information(this, tr("TCP Client"),
//                                 tr("The following error occurred: %1.")
//                                 .arg(tcpSocket->errorString()));
    }

}
