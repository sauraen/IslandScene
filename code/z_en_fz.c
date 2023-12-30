#include "z_en_fz.h"
#include "assets/objects/object_fz/object_fz.h"
#include "assets/scenes/dungeons/MIZUsin_bs/MIZUsin_bs_scene.h"

#define FLAGS 0x00000030

void EnFz_Init(Actor* thisx, PlayState* play);
void EnFz_Destroy(Actor* thisx, PlayState* play);
void EnFz_Update(Actor* thisx, PlayState* play);
void EnFz_Draw(Actor* thisx, PlayState* play);

ActorInit En_Fz_InitVars = {
    ACTOR_EN_FZ,
    ACTORCAT_PROP,
    FLAGS,
    OBJECT_FZ,
    sizeof(EnFz),
    (ActorFunc)EnFz_Init,
    (ActorFunc)EnFz_Destroy,
    (ActorFunc)EnFz_Update,
    (ActorFunc)EnFz_Draw,
};

void EnFz_Init(Actor* thisx, PlayState* play) {
    //EnFz* this = (EnFz*)thisx;

}

void EnFz_Destroy(Actor* thisx, PlayState* play) {
    //EnFz* this = (EnFz*)thisx;

}

#define DL_BEACH_BALL_SPECULAR 0
#define DL_BEACH_BALL_TEX_SPECULAR 1
#define DL_BEACH_BALL_HILITE 2
#define DL_PUNCH_BOWL 3
#define DL_TURTLE 4
#define DL_BUMP_MAP_SPHERE 5
#define NUM_DLS 6
static Gfx* displayLists[NUM_DLS] = {
    BeachBallSpecular,
    BeachBallTexSpecular,
    BeachBallHilite,
    PunchBowl,
    Turtle,
    BumpMapSphere,
};

void EnFz_Update(Actor* thisx, PlayState* play) {
    EnFz* this = (EnFz*)thisx;
    s16 param = this->actor.params;
    s16 rot = (param <= DL_BEACH_BALL_HILITE) ? 0x180 : 0x80;
    this->actor.shape.rot.y += rot;
    
    Input* input = &play->state.input[0];
    bool btn = CHECK_BTN_ALL(input->press.button, BTN_DUP);
    if(param == 0 && btn){
        // Generate YouTube thumbnail
        play->csCtx.script = SEGMENTED_TO_VIRTUAL(gIslandSceneCs);
		gSaveContext.cutsceneTrigger = 1;
    }
}

void EnFz_Draw(Actor* thisx, PlayState* play) {
    EnFz* this = (EnFz*)thisx;
    s16 param = this->actor.params;
    if(param >= NUM_DLS) return;
    if(param == DL_BEACH_BALL_HILITE){
        func_8002EBCC(&this->actor, play, 1); // set up hilite in segment 7
    }
    
    OPEN_DISPS(play->state.gfxCtx);
    
    if(param == DL_BEACH_BALL_HILITE){
        gSPLightToPrimColor(POLY_OPA_DISP++, 1, 0xFF, 0, 0);
    }else if(param == DL_TURTLE){
        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 0, 0xFF, 0, 0xFF);
    }else if(param == DL_BUMP_MAP_SPHERE){
        gSPLightToFogColor(POLY_OPA_DISP++, 1, 0);
    }
    
    if(param == DL_PUNCH_BOWL){
        Gfx_DrawDListXlu(play, displayLists[param]);
    }else{
        Gfx_DrawDListOpa(play, displayLists[param]);
    }
    CLOSE_DISPS(play->state.gfxCtx);
}
