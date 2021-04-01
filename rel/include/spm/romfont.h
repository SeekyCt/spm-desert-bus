#pragma once

#include <types.h>
#include <wii/types.h>

namespace spm::romfont {

extern "C" {

extern const char ** msg_tbl[8];

const char * romFontGetMessage(s32 id);
void romFontInit();
void romFontMake();
void romFontPrintGX(f32 x, f32 y, f32 scale, const wii::RGBA * colour, const char * msg);
int romFontGetWidth(const char * message);

}

}