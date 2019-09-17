
// Author   KMS - Martin Dubois, ing.
// Product  BinCom
// File     BinCom/CheckSum.h

#pragma once

// Includes
/////////////////////////////////////////////////////////////////////////////

// ===== Import/Includes ====================================================
#include <KmsLib/Windows/ComPortHandle.h>

// Class
/////////////////////////////////////////////////////////////////////////////

class CheckSum
{

public:

    virtual ~CheckSum();

    virtual void Compute(const uint8_t * aIn, unsigned int aInSize_byte) = 0;

    virtual void Receive(KmsLib::Windows::ComPortHandle * aHandle) = 0;

    virtual void Reset() = 0;

    virtual void Send(KmsLib::Windows::ComPortHandle * aHandle) = 0;

};
