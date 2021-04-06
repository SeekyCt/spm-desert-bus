#pragma once

#include <types.h>

namespace spm::mario_pouch {

struct PouchCharOrPixlInfo
{
    bool selectable;
    bool selected;
    int16_t itemType;
};

static_assert(sizeof(PouchCharOrPixlInfo) == 0x4);

struct MarioPouchWork
{
    u8 unknown_0x0[0x4 - 0x0];
    int level;
    int attack;
    int hp;
    int maxHp;
    u8 unknown_0x14[0x18 - 0x14];
    int xp;
    int coins;
    int16_t keyItem[32];
    int16_t useItem[10];
    int16_t shopItem[32];
    PouchCharOrPixlInfo characters[4];
    PouchCharOrPixlInfo pixls[16];
    u8 unknown_0x104[0x68c - 0x104];
    int totalCoinsCollected;
    u8 unknown_0x690[0x698 - 0x690];
    int enemiesDefeated;
    u8 unknown_0x69c[0x6a0 - 0x69c];
};

static_assert(sizeof(MarioPouchWork) == 0x6a0);

extern "C" {

MarioPouchWork * pouchGetPtr();
int pouchGetHp();
void pouchAddHp(int increase);
int pouchGetXp();
void pouchAddXp(int increase);

}

}

