/***********************************************************************
* COMPANY :   IPCREATOR
* AUTHOR  :   Tuan TRUONG
*
* FILENAME :  tcp_server.cpp
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
#include "tcp_server.h"
#include "serverthread.h"
#include <QtNetwork>
#include <QtCore>
#include <winsock2.h>
#include <errno.h>

tcp_server::tcp_server(QObject *parent) : QTcpServer(parent)
{

}

tcp_server::~tcp_server()
{

}

void tcp_server::incomingConnection(qintptr socketDescriptor)
{
    ServerThread *thread = new ServerThread(socketDescriptor, nullptr);
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));    
    connect(thread, &ServerThread::received, this, &tcp_server::readAll);
    connect(thread, &ServerThread::connected, this, &tcp_server::newConnectionHandle);
    connect(thread, &ServerThread::disconnected, this, &tcp_server::disconnectHandle);
    connect(this, SIGNAL(sendToClientReq(const char *, int)), thread, SLOT(sendToClient(const char *, int)));
    connect(this, SIGNAL(sendToClientReq(const QString &)), thread, SLOT(sendToClient(const QString &)));
    connect(this, SIGNAL(sendToClientReq(const QByteArray &)), thread, SLOT(sendToClient(const QByteArray &)));
    connect(this, &tcp_server::sendToClientBulkReq, thread, &ServerThread::sendToClientBulk);
    connect(thread, &ServerThread::bytesSent, this, &tcp_server::bytesSentHandle);
    connect(this, SIGNAL(disconnectFromHostReq(void)), thread, SLOT(disconnectFromHostHandle(void)));

    if(thread->isConnected())
    {
        newConnectionHandle();
    }

    thread->start();


}


bool tcp_server:: tcpOpen(QHostAddress address, quint16 port)
{

    bool status = this->listen(address, port);

    if(status == false)
    {
        return false;
    }

    int sockfd = this->socketDescriptor();

    char flag = 1;
    if ( setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(char)) == -1)
    {
        qDebug() << "ERROR: Can't set SO_REUSEADDR";
        exit(EXIT_FAILURE);
    }

    return true;

}

void tcp_server::tcpClose()
{
    /* Demand all server threads to disconnect from the clients */
    emit  disconnectFromHostReq();

    /* Close the socket */
    this->close();


}
void tcp_server::newConnectionHandle()
{
    emit connected(true);

    return;
}

/*!
 * \brief tcp_server::disconnectHandle. Handle client disconnect event.
 */
void tcp_server::disconnectHandle()
{
    emit err_msg("Client has been disconnected.");
    emit connected(false);

    return;
}

/*!
 * \brief tcp_server::readAll Read all data received from the client. Retransmit the message with the signal void received(const QByteArray &data)
 */
void tcp_server::readAll(const QByteArray &data)
{
    emit received(data);

    return;
}

/*!
 * \brief tcp_server::sendToClientDemo. This function is used essentially in the IP tester example design
 */
void tcp_server::sendToClientDemo(const char *data, int pckLen, int nbPck)
{

    emit sendToClientBulkReq(data, pckLen, nbPck);

    return;
}

/*!
 * \brief tcp_server::sendToClient. Send a char array to client
 * \param data is the data array
 * \param len is the array length in number of bytes
 * \return
 */
void tcp_server::sendToClient(const char *data, int len)
{
    emit sendToClientReq(data, len);

}

/*!
 * \brief tcp_server::sendToClient. Send a QString to client
 * \param data is the string too be sent
 * \return
 */
void tcp_server::sendToClient(const QString &data)
{
    emit sendToClientReq(data);
}

/*!
 * \brief tcp_server::sendToClient. Send a QByteArray to client
 * \param data is the data to be sent
 * \return
 */
void tcp_server::sendToClient(const QByteArray &data)
{
    emit sendToClientReq(data);
}

/*!
 * \brief tcp_server::bytesSentHandle. Retransmit the number of bytes sent by client thread
 * \param nbBytes
 */
void tcp_server::bytesSentHandle(int nbBytes)
{
    emit bytesSent(nbBytes);
}

