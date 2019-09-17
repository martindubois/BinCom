
// Author   KMS - Martin Dubois, ing.
// Product  BinCom
// File     BinCom/Display.cpp

// Includes
/////////////////////////////////////////////////////////////////////////////

#include "Component.h"

// ===== C ==================================================================
#include <stdio.h>

// ===== BinCom =============================================================
#include "Display.h"

// Functions
/////////////////////////////////////////////////////////////////////////////

void Display(const char * aIn, unsigned int aInSize_byte)
{
    assert(NULL != aIn);

    for (unsigned int i = 0; i < aInSize_byte; i++)
    {
        switch (aIn[i])
        {
        case '\0': printf("\\0"); break;
        case '\n': printf("\\n"); break;
        case '\r': printf("\\r"); break;
        case '\t': printf("\\t"); break;

        default: printf("%c", aIn[i]);
        }
    }

    printf("\n(%u characteres)\n", aInSize_byte);
}

void Display(const uint8_t * aIn, unsigned int aInSize_byte)
{
    assert(NULL != aIn);

    for (unsigned int i = 0; i < aInSize_byte; i++)
    {
        printf(" %02x", aIn[i]);
    }

    printf("\n(%u bytes)\n", aInSize_byte);
}

