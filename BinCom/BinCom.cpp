
// Author     KMS - Martin Dubois, ing.
// Product    BinCom
// File       BinCom/BinCom.cpp

// Includes
/////////////////////////////////////////////////////////////////////////////

#include "Component.h"

// ===== C ==================================================================
#include <stdio.h>

// ===== Import/Includes ====================================================
#include <KmsLib/Exception.h>
#include <KmsLib/ToolBase.h>
#include <KmsLib/Windows/ComPortHandle.h>
#include <KmsTool.h>

// ===== Common =============================================================
#include "../Common/Version.h"

// ===== BinCom =============================================================
#include "CheckSum_T.h"
#include "Display.h"
#include "Frame_T.h"

// Macros
/////////////////////////////////////////////////////////////////////////////

#define CATCH                                                                          \
    catch (KmsLib::Exception * eE)                                                     \
    {                                                                                  \
        KmsLib::ToolBase::Report(KmsLib::ToolBase::REPORT_ERROR, "Exception");         \
        eE->Write(stdout);                                                             \
    }                                                                                  \
    catch (...)                                                                        \
    {                                                                                  \
        KmsLib::ToolBase::Report(KmsLib::ToolBase::REPORT_ERROR, "Unknown exception"); \
    }

#define VERIFY_OPEN                                                                \
    if (!sComPort.IsOpen())                                                        \
    {                                                                              \
        KmsLib::ToolBase::Report(KmsLib::ToolBase::REPORT_USER_ERROR, "Not open"); \
        return;                                                                    \
    }

// Commands
/////////////////////////////////////////////////////////////////////////////

static void CheckSum_Reset(KmsLib::ToolBase * aToolBase, const char * aArg);

static const KmsLib::ToolBase::CommandInfo CHECK_SUM_COMMANDS[] =
{
    { "Reset", CheckSum_Reset, "Reset", NULL },

    { NULL, NULL, NULL, NULL }
};

static void Frame_Info (KmsLib::ToolBase * aToolBase, const char * aArg);
static void Frame_Reset(KmsLib::ToolBase * aToolBase, const char * aArg);
static void Frame_Set  (KmsLib::ToolBase * aToolBase, const char * aArg);

static const KmsLib::ToolBase::CommandInfo FRAME_COMMANDS[] =
{
    { "Info" , Frame_Info , "Info"   , NULL },
    { "Reset", Frame_Reset, "Reset"  , NULL },
    { "Set"  , Frame_Set  , "Set ...", NULL },

    { NULL, NULL, NULL, NULL }
};

static void Receive_Binary  (KmsLib::ToolBase * aToolBase, const char * aArg);
static void Receive_CheckSum(KmsLib::ToolBase * aToolBase, const char * aArg);
static void Receive_Frame   (KmsLib::ToolBase * aToolBase, const char * aArg);
static void Receive_Text    (KmsLib::ToolBase * aToolBase, const char * aArg);

static const KmsLib::ToolBase::CommandInfo RECEIVE_COMMANDS[] =
{
    { "Binary"  , Receive_Binary  , "Binary {Size_byte}", NULL },
    { "CheckSum", Receive_CheckSum, "CheckSum"          , NULL },
    { "Frame"   , Receive_Frame   , "Frame"             , NULL },
    { "Text"    , Receive_Text    , "Text {Size_byte}"  , NULL },

    { NULL, NULL, NULL, NULL }
};

static void Send_Binary  (KmsLib::ToolBase * aToolBase, const char * aArg);
static void Send_CheckSum(KmsLib::ToolBase * aToolBase, const char * aArg);
static void Send_Frame   (KmsLib::ToolBase * aToolBase, const char * aArg);
static void Send_Text    (KmsLib::ToolBase * aToolBase, const char * aArg);

static const KmsLib::ToolBase::CommandInfo SEND_COMMANDS[] =
{
    { "Binary"  , Send_Binary  , "Binary {Data}", NULL },
    { "CheckSum", Send_CheckSum, "CheckSum"     , NULL },
    { "Frame"   , Send_Frame   , "Frame {Data}" , NULL },
    { "Text"    , Send_Text    , "Text {Text}"  , NULL },

    { NULL, NULL, NULL, NULL }
};

static void Close(KmsLib::ToolBase * aToolBase, const char * aArg);
static void Open (KmsLib::ToolBase * aToolBase, const char * aArg);

static const KmsLib::ToolBase::CommandInfo COMMANDS[] =
{
    { "CheckSum"     , NULL                           , "CheckSum ..."         , CHECK_SUM_COMMANDS },
    { "Close"        , Close                          , "Close"                , NULL               },
    { "ExecuteScript", KmsLib::ToolBase::ExecuteScript, "ExecuteSript {Script}", NULL               },
    { "Exit"         , KmsLib::ToolBase::Exit         , "Exit"                 , NULL               },
    { "Frame"        , NULL                           , "Frame ..."            , FRAME_COMMANDS     },
    { "Open"         , Open                           , "Open {COM99}"         , NULL               },
    { "Receive"      , NULL                           , "Receive ..."          , RECEIVE_COMMANDS   },
    { "Send"         , NULL                           , "Send ..."             , SEND_COMMANDS      },

    { NULL, NULL, NULL, NULL }
};

// Static function declaration
/////////////////////////////////////////////////////////////////////////////

static void Receive_Binary(unsigned int aSize_byte);

static void Send_Binary(const uint8_t * aIn, unsigned int aInSize_byte);

static unsigned int TextToBinary(const char * aIn, unsigned char * aOut);

// Static variables
/////////////////////////////////////////////////////////////////////////////

static CheckSum_T sCheckSum_T;

static CheckSum * sCheckSum = &sCheckSum_T;

static KmsLib::Windows::ComPortHandle sComPort;

static Frame_T sFrame_T;

static Frame * sFrame = &sFrame_T;

// Entry point
/////////////////////////////////////////////////////////////////////////////

int main(int aCount, const char ** aVector)
{
    KMS_TOOL_BANNER("BinCom", "BinCom", VERSION_STR, VERSION_TYPE);

    try
    {
        sComPort.SetBaudRate(KmsLib::Windows::ComPortHandle::BAUD_RATE_115200);

        KmsLib::ToolBase lToolBase(COMMANDS);

        if (!lToolBase.ParseArguments(aCount, aVector))
        {
            lToolBase.ParseCommands();
        }
    }
    catch (KmsLib::Exception * eE)
    {
        KmsLib::ToolBase::Report( KmsLib::ToolBase::REPORT_FATAL_ERROR, "Exception");
        eE->Write(stdout);
    }
    catch (...)
    {
        KmsLib::ToolBase::Report(KmsLib::ToolBase::REPORT_FATAL_ERROR, "Unknown exception");
    }

    return 0;
}

// Commands
/////////////////////////////////////////////////////////////////////////////

void CheckSum_Reset(KmsLib::ToolBase * aToolBase, const char * aArg)
{
    assert(NULL != sCheckSum);

    sCheckSum->Reset();

    KmsLib::ToolBase::Report(KmsLib::ToolBase::REPORT_OK, "Reset");
}

void Frame_Info(KmsLib::ToolBase * aToolBase, const char * aArg)
{
    assert(NULL != sFrame);

    sFrame->Info();
}

void Frame_Reset(KmsLib::ToolBase * aToolBase, const char * aArg)
{
    assert(NULL != sFrame);

    sFrame->Reset();

    KmsLib::ToolBase::Report(KmsLib::ToolBase::REPORT_OK, "Reset");
}

void Frame_Set(KmsLib::ToolBase * aToolBase, const char * aArg)
{
    assert(NULL != aArg);

    assert(NULL != sFrame);

    sFrame->Set(aArg);
}

void Receive_Binary(KmsLib::ToolBase * aToolBase, const char * aArg)
{
    assert(NULL != aArg);

    unsigned int lSize_byte;

    switch (sscanf_s(aArg, "%u", &lSize_byte))
    {
    case EOF: lSize_byte = 1; break;

    case 1:
        if (0 >= lSize_byte)
        {
            KmsLib::ToolBase::Report(KmsLib::ToolBase::REPORT_USER_ERROR, "Invalid size");
            return;
        }
        break;

    default:
        KmsLib::ToolBase::Report(KmsLib::ToolBase::REPORT_USER_ERROR, "Invalid argument");
        return;
    }

    VERIFY_OPEN

    try
    {
        Receive_Binary(lSize_byte);
    }
    CATCH
}

void Receive_CheckSum(KmsLib::ToolBase * aToolBase, const char * aArg)
{
    VERIFY_OPEN

    try
    {
        sCheckSum->Receive(&sComPort);
    }
    CATCH
}

void Receive_Frame(KmsLib::ToolBase * aToolBase, const char * aArg)
{
    assert(NULL != aArg);

    unsigned int lSize_byte;

    switch (sscanf_s(aArg, "%u", &lSize_byte))
    {
    case EOF: lSize_byte = 0; break;

    case 1:
        break;

    default:
        KmsLib::ToolBase::Report(KmsLib::ToolBase::REPORT_USER_ERROR, "Invalid argument");
        return;
    }

    VERIFY_OPEN

    try
    {
        sFrame->Receive(&sComPort, lSize_byte);
    }
    CATCH
}

void Receive_Text(KmsLib::ToolBase * aToolBase, const char * aArg)
{
    assert(NULL != aArg);

    assert(NULL != sCheckSum);

    unsigned int lSize_byte;

    switch (sscanf_s(aArg, "%u", &lSize_byte))
    {
    case EOF: lSize_byte = 1; break;

    case 1:
        if (0 >= lSize_byte)
        {
            KmsLib::ToolBase::Report(KmsLib::ToolBase::REPORT_USER_ERROR, "Invalid size");
            return;
        }
        break;

    default:
        KmsLib::ToolBase::Report(KmsLib::ToolBase::REPORT_USER_ERROR, "Invalid argument");
        return;
    }

    VERIFY_OPEN

    char * lBuffer = new char[lSize_byte];

    try
    {
        unsigned int lInfo_byte = sComPort.Read(lBuffer, lSize_byte);
        assert(lSize_byte >= lInfo_byte);

        sCheckSum->Compute(reinterpret_cast<uint8_t *>(lBuffer), lInfo_byte);

        Display(lBuffer, lSize_byte);
    }
    CATCH

    delete [] lBuffer;
}

void Send_Binary(KmsLib::ToolBase * aToolBase, const char * aArg)
{
    assert(NULL != aArg);

    unsigned char lBuffer[4096];

    unsigned int lSize_byte = TextToBinary(aArg, lBuffer);
    if (0 >= lSize_byte)
    {
        KmsLib::ToolBase::Report(KmsLib::ToolBase::REPORT_USER_ERROR, "Invalid data");
        return;
    }

    VERIFY_OPEN

    try
    {
        Send_Binary(lBuffer, lSize_byte);

        KmsLib::ToolBase::Report(KmsLib::ToolBase::REPORT_OK, "Sent");
    }
    CATCH
}

void Send_CheckSum(KmsLib::ToolBase * aToolBase, const char * aArg)
{
    VERIFY_OPEN

    try
    {
        sCheckSum->Send(&sComPort);

        KmsLib::ToolBase::Report(KmsLib::ToolBase::REPORT_OK, "Sent");
    }
    CATCH
}

void Send_Frame(KmsLib::ToolBase * aToolBase, const char * aArg)
{
    assert(NULL != aArg);

    unsigned char lBuffer[4096];

    unsigned int lSize_byte = TextToBinary(aArg, lBuffer);

    VERIFY_OPEN

    try
    {
        sFrame->Send(&sComPort, lBuffer, lSize_byte);

        KmsLib::ToolBase::Report(KmsLib::ToolBase::REPORT_OK, "Sent");
    }
    CATCH
}

void Send_Text(KmsLib::ToolBase * aToolBase, const char * aArg)
{
    assert(NULL != aArg);

    assert(NULL != sCheckSum);

    size_t lSize_byte = strlen(aArg);
    if (0 >= lSize_byte)
    {
        KmsLib::ToolBase::Report(KmsLib::ToolBase::REPORT_USER_ERROR, "Empty text");
        return;
    }

    VERIFY_OPEN

    try
    {
        sCheckSum->Compute(reinterpret_cast<const uint8_t *>(aArg), lSize_byte);
        sComPort  .Write  (                                  aArg , lSize_byte);

        KmsLib::ToolBase::Report(KmsLib::ToolBase::REPORT_OK, "Sent");
    }
    CATCH
}

void Close(KmsLib::ToolBase * aToolBase, const char * aArg)
{
    VERIFY_OPEN

    try
    {
        sComPort.Close();

        KmsLib::ToolBase::Report(KmsLib::ToolBase::REPORT_OK, "Closed");
    }
    CATCH
}

void Open(KmsLib::ToolBase * aToolBase, const char * aArg)
{
    assert(NULL != aArg);

    char lName[8];

    switch (sscanf_s(aArg, "%[COM1-9]", lName, sizeof(lName)))
    {
    case 1: break;

    default:
        KmsLib::ToolBase::Report(KmsLib::ToolBase::REPORT_USER_ERROR, "Invalid argument");
        return;
    }

    if (sComPort.IsOpen())
    {
        KmsLib::ToolBase::Report(KmsLib::ToolBase::REPORT_USER_ERROR, "Already open");
        return;
    }

    try
    {
        sComPort.Connect(lName, GENERIC_READ | GENERIC_WRITE);

        KmsLib::ToolBase::Report(KmsLib::ToolBase::REPORT_OK, "Open");
    }
    CATCH
}

// Static function
/////////////////////////////////////////////////////////////////////////////

void Receive_Binary(unsigned int aSize_byte)
{
    uint8_t lBuffer[4096];

    assert(              0 <  aSize_byte);
    assert(sizeof(lBuffer) >= aSize_byte);

    assert(NULL != sCheckSum);

    unsigned int lSize_byte = sComPort.Read(lBuffer, aSize_byte);
    assert(aSize_byte >= lSize_byte);

    sCheckSum->Compute(lBuffer, lSize_byte);

    Display(lBuffer, lSize_byte);
}

void Send_Binary(const uint8_t * aIn, unsigned int aInSize_byte)
{
    assert(NULL != aIn         );
    assert(   0 <  aInSize_byte);

    assert(NULL != sCheckSum);

    sCheckSum->Compute(aIn, aInSize_byte);
    sComPort  .Write  (aIn, aInSize_byte);
}

unsigned int TextToBinary(const char * aIn, unsigned char * aOut)
{
    assert(NULL != aIn );
    assert(NULL != aOut);

    unsigned int  lCount  = 0;
    unsigned int  lIn     = 0;
    unsigned int  lResult = 0;
    unsigned char lValue  = 0;

    for (;;)
    {
        switch (aIn[lIn])
        {
        case '\0':
            if (0 < lCount) { aOut[lResult] = lValue; lCount = 0; lResult++; lValue = 0; }
            return lResult;

        case ' ' : case '\t':
            if (0 < lCount) { aOut[lResult] = lValue; lCount = 0; lResult++; lValue = 0; }
            break;

        case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
            lCount++; lValue <<= 4; lValue |= aIn[lIn]      - '0';
            break;

        case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
            lCount++; lValue <<= 4; lValue |= aIn[lIn] + 10 - 'A';
            break;

        case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
            lCount++; lValue <<= 4; lValue |= aIn[lIn] + 10 - 'a';
            break;

        default:
            return 0;
        }

        if (2 <= lCount) { aOut[lResult] = lValue; lCount = 0; lResult++; lValue = 0; }

        lIn++;
    }
}
