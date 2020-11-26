/***********************************************************************
* COMPANY :   IPCREATOR
* AUTHOR  :   Tuan TRUONG
*
* FILENAME :  prbsgenerator10g.h
*
* DESCRIPTION :
*     PRBSGenerator10G class. Manage the PRBS sequence used in FPGA IP stack 10G test bench
*
* CHANGES :
*
* VERSION	DATE		WHO			DETAIL
* 2.0 		11/26/20 	TT      	Initial file creation
*
*/
#ifndef PRBSGENERATOR10G_H
#define PRBSGENERATOR10G_H

#include <QObject>
#include <QVector>

class PRBSGenerator10G : public QObject
{
    Q_OBJECT
public:
    explicit PRBSGenerator10G(QObject *parent = nullptr);

    // Set the seed
    void setSeed(uint64_t seed){ mSeed = seed; mLFSR = seed; }
    // Reset the generator
    void reset(){ mLFSR = mSeed; }
    void generate(QVector<uint8_t> &out, int len);
    void generateChar(char *out, int len);

signals:
private:
    uint64_t mLFSR;
    uint64_t mSeed;
    uint64_t mPolynomial;
};

#endif // PRBSGENERATOR10G_H
