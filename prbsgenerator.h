/***********************************************************************
* COMPANY :   IPCREATOR
* AUTHOR  :   Tuan TRUONG
*
* FILENAME :  prbsgenerator.h
*
* DESCRIPTION :
*     PRBSGenerator class. Manage the PRBS sequence used in FPGA IP stack 1G test bench
*
* CHANGES :
*
* VERSION	DATE		WHO			DETAIL
* 2.0 		11/26/20 	TT      	Initial file creation
*
*/
#ifndef PRBSGENERATOR_H
#define PRBSGENERATOR_H

#include <QObject>
#include <QVector>

class PRBSGenerator : public QObject
{
    Q_OBJECT
public:
    explicit PRBSGenerator(QObject *parent = nullptr);

    // Set the seed
    void setSeed(uint8_t seed){ mSeed = seed; mLFSR = seed; }
    // Reset the generator
    void reset(){ mLFSR = mSeed; }
    void generate(QVector<uint8_t> &out, int len);
    void generateChar(char *out, int len);

signals:

public slots:

private:
    uint8_t mLFSR;
    uint8_t mSeed;
    uint8_t mPolinomial;
};

#endif // PRBSGENERATOR_H
