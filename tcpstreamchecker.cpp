/***********************************************************************
* COMPANY :   IPCREATOR
* AUTHOR  :   Tuan TRUONG
*
* FILENAME :  tcpstreamchecker.cpp
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
#include "tcpstreamchecker.h"

#define     NB_PCK_MAX      ( 1000000 )
#define     MAX_LEN         ( 1400 )

TcpStreamChecker::TcpStreamChecker(QObject *parent) :
    QObject(parent),
    mErrorCount(0),
    mCorrectCount(0),
    mDataLen(0),
    mDataGenerated(false),
    mMode(sm1G)

{
    mData = NULL;

    /* PRBS generator */
    for(int i = 0; i < 1; i++)
    {
        // 1G generator
        PRBSGenerator *gen = new PRBSGenerator(this);

        gen->setSeed(0xAB);

        mPRBSGenList.append(gen);

        // 10G generator
        PRBSGenerator10G *gen10G = new PRBSGenerator10G(this);

        gen10G->setSeed(0xFEEDABBABABEBEEF);

        mPRBSGenList10G.append(gen10G);

    }

    PRBSGenReset();

}

TcpStreamChecker::~TcpStreamChecker()
{
    for(int i = 0; i < mPRBSGenList.length(); i++)
    {
        delete mPRBSGenList[i];
    }

    free(mData);
}

bool TcpStreamChecker::dataMalloc(quint64 len)
{
    free(mData);

    // This data is used to initialize data to sent to client for IP Tester example design
    mData = (char *)malloc(len * sizeof(*mData));
    if(mData == NULL)
    {
        qDebug() << "Cannot allocate memory for the Stream checker.";
        return false;
    }

    return true;
}


/*!
 * \brief TcpStreamChecker::PRBSGenReset. Reset all the PRBS generators
 */
void TcpStreamChecker:: PRBSGenReset()
{
    for(int i = 0; i < mPRBSGenList.length(); i++)
    {
        mPRBSGenList[i]->reset();
    }

    for(int i = 0; i < mPRBSGenList10G.length(); i++)
    {
        mPRBSGenList10G[i]->reset();
    }

    mErrorCount = 0;
    mCorrectCount = 0;

    return;
}

/*!
 * \brief TcpStreamChecker::PRBSByteGenerate. Generate an array of random bytes thanks to the PRBS generators
 * \param out is the output array
 * \param len is the desired array length
 */
void TcpStreamChecker::PRBSByteGenerate(QVector<uint8_t> &out, int len)
{
    if(mMode == sm1G)
    {
        PRBSGenerator *generator = mPRBSGenList.at(0);

        generator->generate(out, len);
    }
    else if(mMode == sm10G)
    {
        PRBSGenerator10G *generator = mPRBSGenList10G.at(0);

        generator->generate(out, len);
    }
    else
    {
        // For future use
    }
}

/*!
 * \brief TcpStreamChecker::PRBSByteGenerate. Generate a vector of len pseudo random bytes. This byte sequence matches
 * that of the IP traffic generator used in TCP/UDP/IP 1G Stack example design.
 * \param out
 * \param len
 */
void TcpStreamChecker::PRBSByteGenerate(char * out, int len)
{
    if(mMode == sm1G)
    {
        PRBSGenerator *generator = mPRBSGenList.at(0);

        generator->generateChar(out, len);
    }
    else if(mMode == sm10G)
    {
        PRBSGenerator10G *generator = mPRBSGenList10G.at(0);

        generator->generateChar(out, len);
    }
    else
    {
        // For future use
    }
}

/*!
 * \brief TcpStreamChecker::streamCheck. Verify the data integrity of the stream
 * \param data
 */
void TcpStreamChecker::streamCheck(const QByteArray &data)
{
    QVector<uint8_t> byteArray(data.length());
    PRBSByteGenerate(byteArray, data.length());

    bool errorFound = false;

    for(int i = 0; i < data.length(); i++)
    {
        uint8_t received = (uint8_t)data.at(i);

        if(received != byteArray.at(i))
        {
            mErrorCount++;
            errorFound = true;
        }
        else
        {
            mCorrectCount++;
        }
    }

    if(errorFound)
    {
        emit errorCount(mErrorCount);
    }
    emit verifCount(mCorrectCount);
    emit message("PRBS checking done.");
}

void TcpStreamChecker::UDPstreamCheck(const char *data, const int nbPck, const int pckLen)
{
    int linkLayerHdrLen = 42;

    bool errorFound = false;

    int offset1 = 0;
    int offset2 = 0;
    for(int i = 0; i < nbPck; i++)
    {


        offset1 += linkLayerHdrLen; // Bypass the linklayer header
        // Extract UDP payload length
        uint16_t plLen = ((((uint16_t)data[offset1-4]&0xFF) << 8) | ((uint16_t)data[offset1-3]&0xFF)) - 8;

        if(plLen > pckLen - linkLayerHdrLen) // This UDP packet is not reliable
        {
            mErrorCount++;
            errorFound = true;
            emit message("Found error UDP packet header. PRBS verification stopped.");
            goto DONE;
        }

        QVector<uint8_t> byteArray(plLen);
        PRBSByteGenerate(byteArray, plLen);

        for(int j = 0; j < plLen; j++)
        {
            uint8_t received = (uint8_t)data[offset1+j];

            if(received != byteArray.at(j))
            {
                mErrorCount++;
                errorFound = true;
            }
            else
            {
                mCorrectCount++;
            }

            offset2++;
        }
        offset1+=pckLen - linkLayerHdrLen;
    }

DONE:
    if(errorFound)
    {
        emit errorCount(mErrorCount);
    }
    emit verifCount(mCorrectCount);
    emit message("PRBS checking done. " + QString::number(mErrorCount) + " bytes error.");
    return;
}

/*!
 * \brief TcpStreamChecker::sendToHostTest. This function is used essentially in IP Tester example design to send data to the host
 */
void TcpStreamChecker::sendToHostTest(int pckLen, int nbPck)
{
    PRBSGenReset();
    quint64 offset = 0;
    if(!mDataGenerated)
    {
        if(!dataMalloc((quint64)pckLen*(quint64)nbPck))
        {
            emit message("Cannot allocate memory for PRBS data.");
            return;
        }
        mDataLen = 0;
        for(int i = 0; i < nbPck; i++)
        {
            PRBSByteGenerate(mData + offset, pckLen);
            offset += pckLen;
            mDataLen += pckLen;
            emit DataGenerating(mDataLen);
        }
        mDataGenerated = true;
    }
    else
    {
        if(mDataLen == (quint64)pckLen*(quint64)nbPck)
        {
            emit DataGenerating(mDataLen);
        }
        else
        {
            if(!dataMalloc((quint64)pckLen*(quint64)nbPck))
            {
                emit message("Cannot allocate memory for PRBS data.");
                return;
            }
            mDataLen = 0;
            for(int i = 0; i < nbPck; i++)
            {
                PRBSByteGenerate(mData + offset, pckLen);
                offset += pckLen;
                mDataLen += pckLen;
                emit DataGenerating(mDataLen);
            }
            mDataGenerated = true;
        }

    }
    emit DataGenerating(mDataLen);

    emit sendToHostReq(mData, pckLen, nbPck);

}
