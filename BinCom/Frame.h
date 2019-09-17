
// Author   KMS - Martin Dubois, ing.
// Product  BinCom
// File     BinCom/Frame.h

#pragma once

// Includes
/////////////////////////////////////////////////////////////////////////////

// ===== Import/Includes ====================================================
#include <KmsLib/Windows/ComPortHandle.h>

// ====== BinCom ============================================================
class CheckSum;

// Class
/////////////////////////////////////////////////////////////////////////////

class Frame
{

public:

    Frame(CheckSum * aCheckSum);

    virtual ~Frame();

    virtual void Info() = 0;

    virtual void Set(const char * aArg);

    virtual void Receive(KmsLib::Windows::ComPortHandle * aComPort, unsigned int aSize_byte) = 0;

    virtual void Reset();

    virtual void Send(KmsLib::Windows::ComPortHandle * aComPort, const uint8_t * aIn, unsigned int aInSize_byte) = 0;

protected:

    void ComputeAndSend(KmsLib::Windows::ComPortHandle * aComPort, const uint8_t * aIn, unsigned int aInSize_byte);

    unsigned int ReceiveAndCompute(KmsLib::Windows::ComPortHandle * aComPort, uint8_t * aOut, unsigned int aOutSize_byte);

    CheckSum * mCheckSum;

};
