
// Author   KMS - Martin Dubois, ing.
// Product  BinCom
// File     BinCom/Frame_T.cpp

// Includes
/////////////////////////////////////////////////////////////////////////////

#include "Component.h"

// ===== C ==================================================================
#include <stdio.h>

// ===== Import/Includes ====================================================
#include "KmsLib/ToolBase.h"

// ===== BinCom =============================================================
#include "Display.h"

#include "Frame_T.h"

// Constants
/////////////////////////////////////////////////////////////////////////////

static const unsigned char END_OF_FRAME  [] = { 0x7f };
static const unsigned char START_OF_FRAME[] = { 0x7e };

// Public
/////////////////////////////////////////////////////////////////////////////

Frame_T::Frame_T() :
    Frame( & mCheckSum_T ),
    mCommand     (0),
    mRegister    (0),
    mSlaveAddress(0),
    mSyncByte    (0)
{
}

// ===== Frame ==============================================================

Frame_T::~Frame_T()
{
}

void Frame_T::Info()
{
    printf(
        "Name         Minimum Default Maximum\n"
        "============ ======= ======= =======\n"
        "Command      0x00    0x00    0xff\n"
        "Register     0x0000  0x0000  0xffff\n"
        "SlaveAddress 0x00    0x00    0xff\n"
        "SyncByte     0x00    0x00    0xff\n"
        "\n"
        "SyncByte is incremented before sending a frame.\n" );
}

void Frame_T::Set(const char * aArg)
{
    assert(NULL != aArg);

    char         lName[16];
    unsigned int lValue;

    switch (sscanf_s(aArg, "%[A-Za-z] %x", lName, sizeof(lName), & lValue))
    {
    case 1 :
        if (0 == _strnicmp("Command"     , lName,  7)) { mCommand      = 0; return; }
        if (0 == _strnicmp("Register"    , lName,  8)) { mRegister     = 0; return; }
        if (0 == _strnicmp("SlaveAddress", lName, 12)) { mSlaveAddress = 0; return; }
        if (0 == _strnicmp("SyncByte"    , lName,  8)) { mSyncByte     = 0; return; }
        break;

    case 2 :
        if (0 == _strnicmp("Command"     , lName,  7)) { mCommand      = lValue; return; }
        if (0 == _strnicmp("Register"    , lName,  8)) { mRegister     = lValue; return; }
        if (0 == _strnicmp("SlaveAddress", lName, 12)) { mSlaveAddress = lValue; return; }
        if (0 == _strnicmp("SyncByte"    , lName,  8)) { mSyncByte     = lValue; return; }
        break;
    }

    Frame::Set(aArg);
}

void Frame_T::Reset()
{
    mCommand      = 0;
    mRegister     = 0;
    mSlaveAddress = 0;
    mSyncByte     = 0;

    Frame::Reset();
}

void Frame_T::Receive(KmsLib::Windows::ComPortHandle * aComPort, unsigned int aSize_byte)
{
    assert(NULL != aComPort  );
    assert(0xff >= aSize_byte);

    assert(NULL != mCheckSum);

    mCheckSum->Reset();

    uint8_t lStartOfFrame[ sizeof( START_OF_FRAME ) ];

    unsigned int lSize_byte = ReceiveAndCompute(aComPort, lStartOfFrame, sizeof(lStartOfFrame));
    if (sizeof(START_OF_FRAME) != lSize_byte)
    {
        KmsLib::ToolBase::Report(KmsLib::ToolBase::REPORT_ERROR, "Cannot read the start of frame");
        return;
    }

    if (0 != memcmp(START_OF_FRAME, lStartOfFrame, sizeof(START_OF_FRAME)))
    {
        KmsLib::ToolBase::Report(KmsLib::ToolBase::REPORT_ERROR, "Invalid start of frame");
        Display(lStartOfFrame, sizeof(lStartOfFrame));
    }

    uint8_t lSlaveAddress;

    lSize_byte = ReceiveAndCompute(aComPort, &lSlaveAddress, sizeof(lSlaveAddress));
    if (sizeof(lSlaveAddress) != lSize_byte)
    {
        KmsLib::ToolBase::Report(KmsLib::ToolBase::REPORT_ERROR, "Cannot read the slave address");
        return;
    }

    printf("Slave Address : 0x%02x\n", lSlaveAddress);

    uint8_t lSyncByte;

    lSize_byte = ReceiveAndCompute(aComPort, &lSyncByte, sizeof(lSyncByte));
    if (sizeof(lSyncByte) != lSize_byte)
    {
        KmsLib::ToolBase::Report(KmsLib::ToolBase::REPORT_ERROR, "Cannot read the sync byte");
        return;
    }

    if (mSyncByte != lSyncByte)
    {
        KmsLib::ToolBase::Report(KmsLib::ToolBase::REPORT_ERROR, "Invalid sync byte");
        printf("Expected = 0x%02x, Received = 0x%02x\n", mSyncByte, lSyncByte);
    }

    uint8_t lStatus;

    lSize_byte = ReceiveAndCompute(aComPort, &lStatus, sizeof(lStatus));
    if (sizeof(lStatus) != lSize_byte)
    {
        KmsLib::ToolBase::Report(KmsLib::ToolBase::REPORT_ERROR, "Cannot read the status");
        return;
    }

    printf("Status : 0x%02x\n", lStatus);

    uint16_t lRegister;

    lSize_byte = ReceiveAndCompute(aComPort, reinterpret_cast<uint8_t *>(&lRegister), sizeof(lRegister));
    if (sizeof(lRegister) != lSize_byte)
    {
        KmsLib::ToolBase::Report(KmsLib::ToolBase::REPORT_ERROR, "Cannot read the register");
        return;
    }

    if (mRegister != lRegister)
    {
        KmsLib::ToolBase::Report(KmsLib::ToolBase::REPORT_ERROR, "Invalid register");
        printf("Expected = 0x%04x, Received = 0x%04x\n", mRegister, lRegister);
    }

    uint8_t lSize;

    lSize_byte = ReceiveAndCompute(aComPort, &lSize, sizeof(lSize));
    if (sizeof(lSize) != lSize_byte)
    {
        KmsLib::ToolBase::Report(KmsLib::ToolBase::REPORT_ERROR, "Cannot read the size");
        return;
    }

    if (aSize_byte != lSize)
    {
        KmsLib::ToolBase::Report(KmsLib::ToolBase::REPORT_ERROR, "Invalid size");
        return;
    }

    uint8_t lBuffer[256];

    lSize_byte = ReceiveAndCompute(aComPort, lBuffer, aSize_byte);
    if (aSize_byte != lSize_byte)
    {
        KmsLib::ToolBase::Report(KmsLib::ToolBase::REPORT_ERROR, "Cannot read the data");
    }

    Display(lBuffer, lSize_byte);

    mCheckSum->Receive(aComPort);

    uint8_t lEndOfFrame[sizeof(END_OF_FRAME)];

    lSize_byte = aComPort->Read(lEndOfFrame, sizeof(lEndOfFrame));
    if (sizeof(END_OF_FRAME) != lSize_byte)
    {
        KmsLib::ToolBase::Report(KmsLib::ToolBase::REPORT_ERROR, "Cannot read the end of frame");
        return;
    }

    if (0 != memcmp(END_OF_FRAME, lEndOfFrame, sizeof(END_OF_FRAME)))
    {
        KmsLib::ToolBase::Report(KmsLib::ToolBase::REPORT_ERROR, "Invalid end of frame");
        Display(lEndOfFrame, sizeof(lEndOfFrame));
    }
}

void Frame_T::Send(KmsLib::Windows::ComPortHandle * aComPort, const uint8_t * aIn, unsigned int aInSize_byte)
{
    assert(NULL != aComPort    );
    assert(0xff >= aInSize_byte);

    assert(NULL != mCheckSum);

    mCheckSum->Reset();

    ComputeAndSend(aComPort, START_OF_FRAME, sizeof(START_OF_FRAME));
    ComputeAndSend(aComPort, &mSlaveAddress, sizeof(mSlaveAddress ));

    mSyncByte++;

    ComputeAndSend(aComPort, &mSyncByte, sizeof(mSyncByte));
    ComputeAndSend(aComPort, &mCommand , sizeof(mCommand ));

    ComputeAndSend(aComPort, reinterpret_cast<uint8_t *>(&mRegister   ), sizeof(mRegister));
    ComputeAndSend(aComPort, reinterpret_cast<uint8_t *>(&aInSize_byte), sizeof(uint8_t  ));

    if (0 < aInSize_byte)
    {
        assert(NULL != aIn);

        ComputeAndSend(aComPort, aIn, aInSize_byte);
    }

    mCheckSum->Send(aComPort);

    aComPort->Write(END_OF_FRAME, sizeof(END_OF_FRAME));
}
