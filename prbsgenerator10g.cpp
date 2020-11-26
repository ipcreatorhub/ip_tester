/***********************************************************************
* COMPANY :   IPCREATOR
* AUTHOR  :   Tuan TRUONG
*
* FILENAME :  prbsgenerator10g.cpp
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
#include "prbsgenerator10g.h"

PRBSGenerator10G::PRBSGenerator10G(QObject *parent) : QObject(parent)
{
    mPolynomial = 0xD800000000000000; // This polynome is used in the FPGA stack. Put the same here
}


/*!
 * \brief PRBSGenerator::generate. Generate an array of pseudo-random bits
 * \param out is the bit array
 * \param len is the array length
 */
void PRBSGenerator10G::generate(QVector<uint8_t> &out, int len)
{

    int idx = 0;

    uint64_t lfsr = mLFSR;

    int bytes_processed = 0;

    while(bytes_processed < len)
    {
        int len_local;
        uint64_t lsb = lfsr & 1u;
        lfsr >>= 1;
        if(lsb)
        {
            lfsr ^= mPolynomial;
        }

        /* Copy to output */
        len_local = (len - bytes_processed > 8)?8:(len - bytes_processed);

        for(int i = 0; i < len_local; i++)
        {
            out[idx] = (uint8_t)(lfsr >> 8*i) & 0xFF;
            idx++;
        }

        bytes_processed += len_local;
    }

    // Update the LFSR state for the next time we continue
    mLFSR = lfsr;

    return;

}

void PRBSGenerator10G::generateChar(char *out, int len)
{
    char *bufPtr = out;

    uint64_t lfsr = mLFSR;

    uint32_t bytes_processed = 0;

    while(bytes_processed < len)
    {
        uint32_t len_local;
        uint64_t lsb = lfsr & 1u;
        lfsr >>= 1;
        if(lsb)
        {
            lfsr ^= mPolynomial;
        }

        /* Copy to output */
        len_local = (len - bytes_processed > 8)?8:(len - bytes_processed);

        memcpy(bufPtr, &lfsr, len_local);
        bufPtr += len_local;
        bytes_processed += len_local;
    }

    // Update the LFSR state for the next time we continue
    mLFSR = lfsr;

    return;
}
