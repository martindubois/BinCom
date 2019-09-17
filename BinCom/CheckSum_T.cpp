
// Author   KMS - Martin Dubois, ing.
// Product  BinCom
// File     BinCom/CheckSum_t.cpp

// Includes
/////////////////////////////////////////////////////////////////////////////

#include "Component.h"

// ====== Import/Includes ===================================================
#include <KmsLib/ToolBase.h>

// ====== BinCom ============================================================
#include "CheckSum_t.h"

// Constants
/////////////////////////////////////////////////////////////////////////////

#define CHECK_SUM_INIT ( 0xffff )

// Public
/////////////////////////////////////////////////////////////////////////////

CheckSum_T::CheckSum_T() : mCheckSum(CHECK_SUM_INIT)
{
}

// ===== CheckSum ===========================================================

CheckSum_T::~CheckSum_T()
{
}

void CheckSum_T::Compute(const uint8_t * aIn, unsigned int aInSize_byte)
{
    assert(NULL != aIn);

    for (unsigned int i = 0; i < aInSize_byte; i++)
    {
        uint8_t lByte = aIn[i];

        lByte ^= (unsigned char)(0xff & mCheckSum);
        lByte ^= lByte << 4;

        mCheckSum = ((((uint16_t)lByte << 8) | (uint8_t)(0xff & (mCheckSum >> 8))) ^ (uint8_t)(lByte >> 4) ^ ((uint16_t)lByte << 3));
    }
}

void CheckSum_T::Receive(KmsLib::Windows::ComPortHandle * aComPort)
{
    assert(NULL != aComPort);

    uint16_t lCheckSum;

    unsigned int lSize_byte = aComPort->Read(&lCheckSum, sizeof(lCheckSum));
    if (sizeof(lCheckSum) != lSize_byte)
    {
        KmsLib::ToolBase::Report(KmsLib::ToolBase::REPORT_ERROR, "Cannot read the check sum");
        return;
    }

    if (mCheckSum != lCheckSum)
    {
        KmsLib::ToolBase::Report(KmsLib::ToolBase::REPORT_WARNING, "Check sum doesn't match");
        printf("Expected = %04x, Received = %04x\n", mCheckSum, lCheckSum);
        return;
    }

    KmsLib::ToolBase::Report(KmsLib::ToolBase::REPORT_INFO, "Check sum matchs");
}

void CheckSum_T::Reset()
{
    mCheckSum = CHECK_SUM_INIT;
}

void CheckSum_T::Send(KmsLib::Windows::ComPortHandle * aComPort)
{
    assert(NULL != aComPort);

    aComPort->Write(&mCheckSum, sizeof(mCheckSum));

    Reset();
}
