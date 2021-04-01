#pragma once

#include <types.h>

#include <wii/types.h>

namespace spm::fontmgr {

#define FONT_HEIGHT 20

extern "C" {

void fontmgrInit();
void FontDrawStart();
void FontDrawStart_alpha(u8 alpha);
void FontDrawEdge();
void FontDrawEdgeOff();
void FontDrawRainbowColor();
void FontDrawRainbowColorOff();
void FontDrawNoise();
void FontDrawNoiseOff();
void FontDrawColorIDX(u32 idx);
void FontDrawColor(const wii::RGBA * color);
void FontDrawColor_();
u32 FontGetDrawColor();
void FontDrawScale(float scale);
void FontDrawCode(float x, float y, u16 code);
void FontDrawCodeMtx(wii::Mtx34 matrix, u16 code);
void FontDrawString(float x, float y, const char * string);
void FontDrawStringVecPitch(wii::Vec3 * position, float pitch, const char * string);
void FontDrawStringMtx(wii::Mtx34 matrix, const char * string);
void FontDrawStringCenterMtx(wii::Mtx34 matrix, const char * string);
void FontDrawMessage(int x, int y, const char * message);
void FontDrawMessageMtx(wii::Mtx34 matrix, const char * message);
u16 hankakuSearch(u8 code);
u16 kanjiSearch(u16 code);
u8 kanjiGetWidth(u16 code);
u16 FontGetMessageWidthLine(const char * message, u16 * outlines);
u16 FontGetMessageWidth(const char * message);
u32 HSV2RGB(u8 hsv[3]);

// more

}

}
