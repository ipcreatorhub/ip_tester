/***********************************************************************
* COMPANY :   IPCREATOR
* AUTHOR  :   Tuan TRUONG
*
* FILENAME :  tcpstreamchecker.h
*
* DESCRIPTION :
*     TcpStreamChecker class. Manage PRBS data stream
*
* CHANGES :
*
* VERSION	DATE		WHO			DETAIL
* 2.0 		11/26/20 	TT      	Initial file creation
*
*/
#ifndef TCPSTREAMCHECKER_H
#define TCPSTREAMCHECKER_H

#include <QObject>
#include <QList>
#include <QHostAddress>
#include "prbsgenerator.h"
#include "prbsgenerator10g.h"

class TcpStreamChecker : public QObject
{
    Q_OBJECT
public:
    explicit TcpStreamChecker(QObject *parent = nullptr);
    ~TcpStreamChecker();

    enum SpeedMode{
        sm1G, // 1 Gigabit
        sm10G // 10 Gigabit
    };
    Q_ENUMS(SpeedMode)

    /* PRBS Generators */
    void PRBSGenReset();
    void PRBSByteGenerate(QVector<uint8_t> &out, int len);
    void PRBSByteGenerate(char * out, int len);

    /* Helpers */
    bool dataMalloc(quint64 len);
signals:
    void errorCount(quint64 numberErrors);
    void verifCount(quint64 numberCorrects);
    void sendToHostReq(const char *data, int len, int nbPck);
    void DataGenerating(quint64 nbBytes);
    void message(const QString &msg);
public slots:
    void streamCheck(const QByteArray &data);
    void UDPstreamCheck(const char *data, const int nbPck, const int pckLen);
    void sendToHostTest(int pckLen, int nbPck);
    void setSpeedMode(SpeedMode mode){mMode = mode; mDataGenerated = false;}
private:
    QList<PRBSGenerator *> mPRBSGenList;
    QList<PRBSGenerator10G *> mPRBSGenList10G;
    quint64 mErrorCount;
    quint64 mCorrectCount;
    char *mData;
    quint64 mDataLen;
    bool mDataGenerated;
    // 10G mode
    SpeedMode mMode;
};

#endif // TCPSTREAMCHECKER_H
