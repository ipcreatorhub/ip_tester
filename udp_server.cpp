/***********************************************************************
* COMPANY :   IPCREATOR
* AUTHOR  :   Tuan TRUONG
*
* FILENAME :  udp_server.cpp
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
#include "udp_server.h"
#include <QElapsedTimer>
#include <pcap.h>

#ifdef WIN32
#include <tchar.h>
BOOL LoadNpcapDlls()
{
    _TCHAR npcap_dir[512];
    UINT len;
    len = GetSystemDirectory(npcap_dir, 480);
    if (!len) {
        fprintf(stderr, "Error in GetSystemDirectory: %x", GetLastError());
        return FALSE;
    }
    _tcscat_s(npcap_dir, 512, _T("\\Npcap"));
    if (SetDllDirectory(npcap_dir) == 0) {
        fprintf(stderr, "Error in SetDllDirectory: %x", GetLastError());
        return FALSE;
    }
    return TRUE;
}
#endif
/* 4 bytes IP address */
typedef struct ip_address
{
    u_char byte1;
    u_char byte2;
    u_char byte3;
    u_char byte4;
}ip_address;

/* IPv4 header */
typedef struct ip_header
{
    u_char	ver_ihl;		// Version (4 bits) + Internet header length (4 bits)
    u_char	tos;			// Type of service
    u_short tlen;			// Total length
    u_short identification; // Identification
    u_short flags_fo;		// Flags (3 bits) + Fragment offset (13 bits)
    u_char	ttl;			// Time to live
    u_char	proto;			// Protocol
    u_short crc;			// Header checksum
    ip_address	saddr;		// Source address
    ip_address	daddr;		// Destination address
    u_int	op_pad;			// Option + Padding
}ip_header;

/* UDP header*/
typedef struct udp_header
{
    u_short sport;			// Source port
    u_short dport;			// Destination port
    u_short len;			// Datagram length
    u_short crc;			// Checksum
}udp_header;

/* prototype of the packet handler */
void packet_handler(u_char *param, const struct pcap_pkthdr *header, const u_char *pkt_data);

udp_server::udp_server(QObject *parent, QHostAddress address, quint16 port) :
    QObject(parent),
    mHostAddress("127.0.0.1"),
    mHostPort(1024),
    mActualPck(0),
    mOffset(0),
    mPckLen(0),
    mNbPck(0),
    mSockFd(0)
{

    mData = NULL;
    mReceiveBuf = NULL;

    WSADATA wsaData;


    /* The WSAStartup function must be the first Windows Sockets function called by an application or DLL.
     * It allows an application or DLL to specify the version of Windows Sockets required and retrieve
     * details of the specific Windows Sockets implementation. The application or DLL can only issue
     * further Windows Sockets functions after successfully calling WSAStartup.
    */
    if (WSAStartup(MAKEWORD(1,1), &wsaData) == SOCKET_ERROR)
    {
       emit message("Error initialising WSA.");
       exit(EXIT_FAILURE);
    }

#ifdef WIN32
    /* Load Npcap and its functions. */
    if (!LoadNpcapDlls())
    {
        fprintf(stderr, "Couldn't load Npcap\n");
        exit(EXIT_FAILURE);
    }
#endif

    mSockFd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(mSockFd < 0)
    {
        emit message("ERROR: Can't open socket");
        emit message("Error code " + QString::number(WSAGetLastError()));
        //exit(EXIT_FAILURE);
    }

    //Prepare the sockaddr_in structure
    mServAddr.sin_family = AF_INET;
    mServAddr.sin_addr.s_addr = inet_addr(address.toString().toStdString().c_str());
    mServAddr.sin_port = htons( port );

    mTimer = new QTimer(this);
    mTimer->setSingleShot(false);
    mTimer->setInterval(1);

    connect(mTimer, &QTimer::timeout, this, &udp_server::timeOutHandle);

}

udp_server::~udp_server()
{

    delete mTimer;
    free(mReceiveBuf);
    free(mData);
}

bool udp_server::bindSock(const QHostAddress &srcAddress, const quint16 &srcPort, const QHostAddress &dstAddress, const quint16 &dstPort)
{
    if(mSockFd == 0)
    {
        return false;
    }
    mHostAddress = dstAddress.toString();
    mHostPort = dstPort;

    mServAddr.sin_addr.s_addr = inet_addr(srcAddress.toString().toStdString().c_str());;
    mServAddr.sin_port = htons( srcPort );


    if (bind(mSockFd, (struct sockaddr *) &mServAddr, sizeof(sockaddr_in)) < 0)
    {
        emit message("ERROR: can't bind socket");
        //exit(EXIT_FAILURE);
        emit bindResult(false);
        return false;
    }

    emit bindResult(true);

    return true;

}


/*!
 * \brief udp_server::bytesWrittenHandle. Retransmit the number of bytes written to the network.
 * \param nbBytes
 */
void udp_server::bytesWrittenHandle(qint64 nbBytes)
{
    emit bytesWritten(nbBytes);

    return;
}

/*!
 * \brief udp_server::readPendingDatagrams. Read all the datagrams and retransmit them via the signal received(QNetworkDatagram)
 */
void udp_server::readPendingDatagrams()
{
    int byteReceived;
    char receiveBuf[1500];
    int bufLength = 1499;
    struct sockaddr_in senderAddr;
    int senderAddrSize;

    byteReceived = recvfrom(mSockFd, receiveBuf, bufLength,
                            0, (struct sockaddr *)&senderAddr, &senderAddrSize);

    emit received(receiveBuf, 1, byteReceived);

    return;
}

/*!
 * \brief udp_server::sendDatagram. Send a datagram to host
 * \param data is the data array to be sent
 * \param len is the array length
 * \param host is the destination IP address
 * \param port is the destination UDP port
 */
void udp_server::sendDatagram(const char *data, int len, QHostAddress host, quint16 port)
{
    struct sockaddr_in si_other;

    //setup address structure
    memset((char *) &si_other, 0, sizeof(si_other));
    si_other.sin_family = AF_INET;
    si_other.sin_port = htons(port);
    //si_other.sin_addr.S_un.S_addr = inet_addr(host.toString().toStdString().c_str());
    si_other.sin_addr.s_addr = inet_addr(host.toString().toStdString().c_str());

    int slen = sizeof(si_other) ;

    if (sendto(mSockFd, data, len , 0 , (struct sockaddr *) &si_other, slen) == SOCKET_ERROR)
    {
        emit message("Sendto failed with error code " + QString::number(WSAGetLastError()));
        //exit(EXIT_FAILURE);
    }

    emit bytesWritten(len);

    return;
}

/*!
 * \brief udp_server::sendDatagramBulk. This function is used essentially in the IP Tester example design to send a bulk of datagrams to destination
 * \param data is the data array.
 * \param pckLen is the datagram length
 * \param nbPck is the number of datagrams
 * \param host is the destination IP address
 * \param port is the destination UDP port
 */
void udp_server::sendDatagramBulk(const char *data, int pckLen, int nbPck)
{
    if(mData != NULL)
    {
        free(mData);
    }

    mData = (char *)malloc((uint64_t)pckLen * (uint64_t)nbPck * (uint64_t)sizeof(*mData));

    if(mData == NULL)
    {
        emit message("Fatal error. Cannot allocate memory for sending UDP data.");
        return;
    }

    memcpy(mData, data, (uint64_t)pckLen * (uint64_t)nbPck * (uint64_t)sizeof(*mData));

    mActualPck = 0;
    mOffset = 0;
    mPckLen = pckLen;
    mNbPck = nbPck;

    mTimer->start();
    emit message("Sending...");

    return;
}

static int recvfromTimeOutUDP(SOCKET socket, long sec, long usec)

{

    // Setup timeval variable

    struct timeval timeout;

    struct fd_set fds;



    timeout.tv_sec = sec;

    timeout.tv_usec = usec;

    // Setup fd_set structure

    FD_ZERO(&fds);

    FD_SET(socket, &fds);

    // Return value:

    // -1: error occurred

    // 0: timed out

    // > 0: data ready to be read

    return select(0, &fds, 0, 0, &timeout);

}

void udp_server::receiveHandle(int nbPck, int len)
{
    int totalBytesReceived = 0;

    free(mReceiveBuf);
    mReceiveBuf = NULL;

    if((len < 0) || (len > 1500))
    {
        emit message("udp_server::receiveHandle. ERROR len is out of range.");
        return;
    }

    mReceiveBuf = (char *)malloc(nbPck * 16384);

    if(mReceiveBuf == NULL)
    {
        emit message("udp_server::receiveHandle. ERROR Not enough memory.");
        return;
    }

    pcap_if_t *alldevs;
    pcap_if_t *d;
    pcap_t *adhandle;
    struct bpf_program fcode;
    u_int netmask;

    int i = 0;

    char errbuf[PCAP_ERRBUF_SIZE];

    QString filter = "udp dst port " + QString::number(htons(mServAddr.sin_port));

    /* Retrieve the device list */
    if(pcap_findalldevs(&alldevs, errbuf) == -1)
    {
        fprintf(stderr,"Error in pcap_findalldevs: %s\n", errbuf);
        exit(EXIT_FAILURE);
    }

    /* Print the list */
    for(d=alldevs; d; d=d->next)
    {
        printf("%d. %s", ++i, d->name);
        if (d->description)
            printf(" (%s)\n", d->description);
        else
            printf(" (No description available)\n");
    }

    int nbDev = i;

    /* Retrive the right device */
    int inum = 0;
    int iaddridx = 0;
    bool found = false;
    for(d=alldevs, i=0; i< nbDev-1 ;d=d->next, i++)
    {
        if(d->addresses)
        {
            int j = 0;
            pcap_addr_t *a;
            for(a=d->addresses;a;a=a->next)
            {
                struct sockaddr_in *addr = (struct sockaddr_in *)(a->addr);
                if(addr->sin_addr.S_un.S_addr == mServAddr.sin_addr.S_un.S_addr)
                {
                    // Found
                    found = true;
                    inum = i+1;
                    iaddridx = j+1;
                    break;
                }
                j++;
            }

        }
        if(found)
        {
            break;
        }

    }

    if(!found)
    {
        emit message("Cannot find the device to listen to.");
        /* Free the device list */
        pcap_freealldevs(alldevs);
        return;
    }

    /* Jump to the adapter */
    for(d=alldevs, i=0; i< inum-1 ;d=d->next, i++);

    /* Open the adapter */
    if ((adhandle= pcap_open_live(d->name,	// name of the device
                             65536,			// portion of the packet to capture.
                                            // 65536 grants that the whole packet will be captured on all the MACs.
                             1,				// promiscuous mode (nonzero means promiscuous)
                             1000,			// read timeout
                             errbuf			// error buffer
                             )) == NULL)
    {
        fprintf(stderr,"\nUnable to open the adapter: %s\n", errbuf);
        /* Free the device list */
        pcap_freealldevs(alldevs);
        return;
    }

    /* Check the link layer. We support only Ethernet for simplicity. */
    if(pcap_datalink(adhandle) != DLT_EN10MB)
    {
        fprintf(stderr,"\nThis program works only on Ethernet networks.\n");
        /* Free the device list */
        pcap_freealldevs(alldevs);
        return;
    }

    if(d->addresses != NULL)
    {
        /* Retrieve the mask of the first address of the interface */
        pcap_addr_t *a;
        for(a=d->addresses, i = 0; i < iaddridx-1; a=a->next, i++);
        netmask=((struct sockaddr_in *)(a->netmask))->sin_addr.S_un.S_addr;
    }
    else
    {
        /* If the interface is without addresses we suppose to be in a C class network */
        netmask=0xffffff;
    }

    //compile the filter
    if (pcap_compile(adhandle, &fcode, filter.toStdString().c_str(), 1, netmask) <0 )
    {
        fprintf(stderr,"\nUnable to compile the packet filter. Check the syntax.\n");
        /* Free the device list */
        pcap_freealldevs(alldevs);
        return;
    }

    //set the filter
    if (pcap_setfilter(adhandle, &fcode)<0)
    {
        fprintf(stderr,"\nError setting the filter.\n");
        /* Free the device list */
        pcap_freealldevs(alldevs);
        return;
    }

    emit message("Npcap engine is listening on device " + QString(d->description));
    emit message("Npcap filter is set to: " + filter);

    /* At this point, we don't need any more the device list. Free it */
    pcap_freealldevs(alldevs);

    /* start the capture */
    int res;
    struct pcap_pkthdr *header;
    const u_char *pkt_data;
    int pckCnt = 0;
    int pckLen = 0;
    while(true)
    {
        res = pcap_next_ex( adhandle, &header, &pkt_data);
        // Copy into interal buffer
        if((res > 0) && (header->len > 0))
        {
            memcpy(mReceiveBuf + totalBytesReceived, pkt_data, header->len);
            totalBytesReceived += header->len;
            pckCnt++;
            emit receivedPck(header->len);
        }

        if(pckCnt == nbPck)
        {
            pckLen = header->len;
            break;
        }
    }

    emit message("UDP Receive done.");

    emit received(mReceiveBuf, nbPck, pckLen);

    return;

}


void udp_server::timeOutHandle()
{
    mTimer->blockSignals(true);
    QHostAddress host = QHostAddress(mHostAddress);
    quint16 port = mHostPort;

    for(int i = 0; i < 1000; i++)
    {
        sendDatagram(mData+mOffset, mPckLen, host, port);
        mOffset += mPckLen;
        mActualPck += 1;

        if(mActualPck == mNbPck)
        {
            mActualPck = 0;
            mTimer->stop();
            free(mData);
            mData = NULL;
            emit message("UDP Transmit done.");
            goto CLEANUP;
        }
    }
CLEANUP:
    mTimer->blockSignals(false);

    return;
}
