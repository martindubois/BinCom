
// Author   KMS - Martin Dubois, ing.
// Product  BinCom
// File     BinCom/Frame.cpp

// Includes
/////////////////////////////////////////////////////////////////////////////

#include "Component.h"

// ===== Import/Includes ====================================================
#include <KmsLib/ToolBase.h>

// ===== BinCom =============================================================
#include "CheckSum.h"

#include "Frame.h"

// Public
/////////////////////////////////////////////////////////////////////////////

Frame::~Frame()
{
}

Frame::Frame(CheckSum * aCheckSum) : mCheckSum(aCheckSum)
{
    assert(NULL != aCheckSum);
}

void Frame::Set(const char * aArg)
{
    KmsLib::ToolBase::Report(KmsLib::ToolBase::REPORT_USER_ERROR, "Invalid argument");
}

void Frame::Reset()
{
}

// Protected
/////////////////////////////////////////////////////////////////////////////

void Frame::ComputeAndSend(KmsLib::Windows::ComPortHandle * aComPort, const uint8_t * aIn, unsigned int aInSize_byte)
{
    assert(NULL != aComPort    );
    assert(NULL != aIn         );
    assert(   0 <  aInSize_byte);

    mCheckSum->Compute(aIn, aInSize_byte);
    aComPort ->Write  (aIn, aInSize_byte);
}

unsigned int Frame::ReceiveAndCompute(KmsLib::Windows::ComPortHandle * aComPort, uint8_t * aOut, unsigned int aOutSize_byte)
{
    assert(NULL != aComPort     );
    assert(NULL != aOut         );
    assert(   0 <  aOutSize_byte);

    unsigned int lResult_byte = aComPort->Read(aOut, aOutSize_byte);

    mCheckSum->Compute(aOut, lResult_byte);

    return lResult_byte;
}
