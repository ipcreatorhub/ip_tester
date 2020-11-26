/***********************************************************************
* COMPANY :   IPCREATOR
* AUTHOR  :   Tuan TRUONG
*
* FILENAME :  prbsgenerator.cpp
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
#include "prbsgenerator.h"

PRBSGenerator::PRBSGenerator(QObject *parent) : QObject(parent)
{
    mPolinomial = 0xB8;
}

/*!
 * \brief PRBSGenerator::generate. Generate an array of pseudo-random bits
 * \param out is the bit array
 * \param len is the array length
 */
void PRBSGenerator::generate(QVector<uint8_t> &out, int len)
{

    for(int i = 0; i < len; i++)
    {
        uint8_t lsb = mLFSR & 0x1;
        /* Shift */
        mLFSR >>= 1;

        if(lsb)
        {
            mLFSR ^= mPolinomial;
        }

        out[i] = mLFSR;
    }
}

void PRBSGenerator::generateChar(char *out, int len)
{
    for(int i = 0; i < len; i++)
    {
        uint8_t lsb = mLFSR & 0x1;
        /* Shift */
        mLFSR >>= 1;

        if(lsb)
        {
            mLFSR ^= mPolinomial;
        }

        out[i] = mLFSR;
    }
}
