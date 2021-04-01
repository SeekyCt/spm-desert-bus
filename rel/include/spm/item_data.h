#pragma once

#include <types.h>

namespace spm::item_data {

#define ITEM_DATA_COUNT 538

struct ItemData 
{
    const char * itemName;
    s16 iconId;
    u8 unknown_0x6[0x8 - 0x6];
    const char * animPoseName;
    u8 unknown_0xc[0x10 - 0xc];
    const char * nameMsg;
    const char * descMsg;
    u8 unknown_0x18[0x2c - 0x18];
};

extern "C" {

extern ItemData itemDataTable[ITEM_DATA_COUNT];

}

}