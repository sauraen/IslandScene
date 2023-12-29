/*
 * File: z_en_anubice.c
 * Overlay: ovl_En_Anubice
 * Description: Palm Tree Leaves
 */

#include "z_en_anubice.h"

#define FLAGS 0x00000030

void EnAnubice_Init(Actor* thisx, PlayState* play);
void EnAnubice_Destroy(Actor* thisx, PlayState* play);
void EnAnubice_Update(Actor* thisx, PlayState* play);
void EnAnubice_Draw(Actor* thisx, PlayState* play);

ActorInit En_Anubice_InitVars = {
    ACTOR_EN_ANUBICE,
    ACTORCAT_BG,
    FLAGS,
    OBJECT_ANUBICE,
    sizeof(EnAnubice),
    (ActorFunc)EnAnubice_Init,
    (ActorFunc)EnAnubice_Destroy,
    (ActorFunc)EnAnubice_Update,
    (ActorFunc)EnAnubice_Draw,
};

void EnAnubice_Init(Actor* thisx, PlayState* play) {
    EnAnubice* this = (EnAnubice*)thisx;
    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 20.0f);
    SkelAnime_InitFlex(play, &this->skelAnime, &PalmLeaves, &PalmLeavesGentlebreezeAnim,
        this->jointTable, this->morphTable, PALMLEAVES_NUM_LIMBS);
    this->skelAnime.curFrame = this->actor.params;
}

void EnAnubice_Destroy(Actor* thisx, PlayState* play) {
    EnAnubice* this = (EnAnubice*)thisx;
	SkelAnime_Free(&this->skelAnime, play);
}

void EnAnubice_Update(Actor* thisx, PlayState* play) {
    EnAnubice* this = (EnAnubice*)thisx;
	SkelAnime_Update(&this->skelAnime);
}

void EnAnubice_Draw(Actor* thisx, PlayState* play) {
    EnAnubice* this = (EnAnubice*)thisx;
    SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable,
        PALMLEAVES_NUM_LIMBS, NULL, NULL, this);
}
