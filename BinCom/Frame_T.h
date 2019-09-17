
// Author   KMS - Martin Dubois, ing.
// Product  BinCom
// File     BinCom/Frame_T.h

#pragma once

// Includes
/////////////////////////////////////////////////////////////////////////////

// ===== BinCom =============================================================
#include "CheckSum_T.h"
#include "Frame.h"

// Class
/////////////////////////////////////////////////////////////////////////////

class Frame_T : public Frame
{

public:

    Frame_T();

    // ===== Frame ==========================================================

    virtual ~Frame_T();

    virtual void Info();

    virtual void Set(const char * aArg);

    virtual void Receive(KmsLib::Windows::ComPortHandle * aComPort, unsigned int aSize_byte);

    virtual void Reset();

    virtual void Send(KmsLib::Windows::ComPortHandle * aComPort, const uint8_t * aIn, unsigned int aInSize_byte);

private:

    CheckSum_T mCheckSum_T  ;
    uint8_t    mCommand     ;
    uint16_t   mRegister    ;
    uint8_t    mSlaveAddress;
    uint8_t    mSyncByte    ;

};
