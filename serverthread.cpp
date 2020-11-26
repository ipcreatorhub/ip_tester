/***********************************************************************
* COMPANY :   IPCREATOR
* AUTHOR  :   Tuan TRUONG
*
* FILENAME :  serverthread.cpp
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
#include "serverthread.h"
#include <QtNetwork>
#include <QObject>
#include <QTcpServer>

ServerThread::ServerThread(int socketDescriptor, QObject *parent)
    : QThread(parent),
      socketDescriptor(socketDescriptor)
{
    tcpSocket = new QTcpSocket();

    tcpSocket->setSocketDescriptor(socketDescriptor);

    /* If the session is finished delete the socket */
    connect(tcpSocket, &QAbstractSocket::disconnected,
            tcpSocket, &QObject::deleteLater);

    /* Read all data coming from the peer */
    connect(tcpSocket, &QAbstractSocket::readyRead,
            this, &ServerThread::readAll);

    /* Update internal connected state and inform the parent */
    connect(tcpSocket, &QAbstractSocket::connected,
            this, &ServerThread::connectedHandle);

    connect(tcpSocket, &QAbstractSocket::disconnected,
            this, &ServerThread::disconnectedHandle);

    /* If the connection has been already established we update the internal state and inform the parent */
    QAbstractSocket::SocketState state = tcpSocket->state();
    qDebug() << state;
    if(state == QAbstractSocket::ConnectedState)
    {
        emit connected();
        mConnected = true;
    }
    else
    {
        mConnected = false;
    }
}

ServerThread::~ServerThread()
{
    delete tcpSocket;
    quit();

    wait();
}

/*!
 * \brief ServerThread::readAll. Read all data coming from the client in the buffer. Emit the signal so that the tcp_server class can receive the data
 */
void ServerThread::readAll()
{
    emit received(tcpSocket->readAll());

    return;
}

/*!
 * \brief ServerThread::connectedHandle. Update internal state and emit the signal in order to inform the parent
 */
void ServerThread::connectedHandle()
{
    emit connected();
    mConnected = true;
}

/*!
 * \brief ServerThread::disconnectedHandle. When the session is finished, update the internal state and emit the signal to inform the parent (tcp server)
 */
void ServerThread::disconnectedHandle()
{
    emit disconnected();
    mConnected = false;
}

/*!
 * \brief ServerThread::sendToClientBulk. Send multiple segments to client
 * \param data is the data array
 * \param pckLen is the segment length
 * \param nbPck is the number of sengment to be sent
 * \return
 */
qint64 ServerThread::sendToClientBulk(const char *data, int pckLen, int nbPck)
{
    qint64 ret = 0;
    int offset = 0;


    for(int i = 0; i < nbPck; i++)
    {
        sendToClient(data + offset, pckLen);
        offset += pckLen;
        ret += pckLen;
    }


    return ret;
}

/*!
 * \brief ServerThread::sendToClient. Send data to peer (client)
 * \param data is the data array
 * \param len is the array length
 * \return the number of bytes sent
 */
qint64 ServerThread::sendToClient(const char *data, int len)
{
    if(mConnected)
    {
        qint64 ret;
        ret = tcpSocket->write(data, len);
        //tcpSocket->waitForBytesWritten();
        emit bytesSent(ret);
        return ret;
    }
    else {
        return 0;
    }
}

/*!
 * \brief ServerThread::sendToClient. Send a string to client
 * \param data is the string to be sent
 * \return
 */
qint64 ServerThread::sendToClient(const QString &data)
{
    if(mConnected)
    {
        qint64 ret;
        ret = tcpSocket->write(data.toUtf8());
        emit bytesSent(ret);
        return ret;
    }
    else {
        return 0;
    }
}

/*!
 * \brief ServerThread::sendToClient. Send a byte array to client.
 * \param data is the array to be sent
 * \return
 */
qint64 ServerThread::sendToClient(const QByteArray &data)
{
    if(mConnected)
    {
        qint64 ret;
        ret = tcpSocket->write(data);
        emit bytesSent(ret);
        return ret;
    }
    else
    {
        return 0;
    }
}

/*!
 * \brief ServerThread::disconnectFromHostHandle. This function is called when the server initiates the closing process.
 */
void ServerThread::disconnectFromHostHandle()
{
    if(mConnected)
    {
        tcpSocket->disconnectFromHost();
    }
}
