
// Author   KMS - Martin Dubois, ing.
// Product  BinCom
// File     BinCom/CheckSum_T.h

#pragma once

// Includes
/////////////////////////////////////////////////////////////////////////////

// ===== BinCom =============================================================
#include "CheckSum.h"

// Class
/////////////////////////////////////////////////////////////////////////////

class CheckSum_T : public CheckSum
{

public:

    CheckSum_T();

    // ===== CheckSum =======================================================

    virtual ~CheckSum_T();

    virtual void Compute(const uint8_t * aIn, unsigned int aInSize_byte);

    virtual void Receive(KmsLib::Windows::ComPortHandle * aHandle);

    virtual void Reset();

    virtual void Send(KmsLib::Windows::ComPortHandle * aHandle);

private:

    uint16_t mCheckSum;

};
