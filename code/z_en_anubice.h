#ifndef Z_EN_ANUBICE_H
#define Z_EN_ANUBICE_H

#include "ultra64.h"
#include "global.h"
#include "assets/objects/object_anubice/object_anubice.h"

struct EnAnubice;

typedef void (*EnAnubiceActionFunc)(struct EnAnubice*, struct PlayState*);

typedef struct EnAnubice {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ SkelAnime skelAnime;
    /* 0x0190 */ Vec3s jointTable[PALMLEAVES_NUM_LIMBS];
    /* 0x01F0 */ Vec3s morphTable[PALMLEAVES_NUM_LIMBS];
} EnAnubice; // size = 0x0314

#endif
