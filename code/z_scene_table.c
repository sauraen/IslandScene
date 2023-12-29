
static Color_RGB8 SampleSky(u8* image, u16* palette, s32 width, s32 line){
    s32 r = 0, g = 0, b = 0;
    for(s32 i=0; i<4; ++i){
        s32 x = (width >> 2) * i;
        u8 ci = image[line * width + x];
        u16 color = palette[ci];
        r += color >> 11;
        g += (color >> 6) & 0x1F;
        b += (color >> 1) & 0x1F;
    }
    // Divide by 4, but each one was only 5 bits instead of 8
    return (Color_RGB8){r << 1, g << 1, b << 1};
}

static u8 LerpU8s(u8 a, u8 b, u8 factor){
    s32 result = (s32)a + ((((s32)b - (s32)a) * factor) / 255);
    if(result < 0) result = 0;
    if(result > 0xFF) result = 0xFF;
    return (u8)result;
}

static Color_RGB8 BlendColors(Color_RGB8 a, Color_RGB8 b, u8 factor){
    return (Color_RGB8){
        LerpU8s(a.r, b.r, factor),
        LerpU8s(a.g, b.g, factor),
        LerpU8s(a.b, b.b, factor),
    };
}

extern Lights* gLastLights;
/*
in z_lights.c:
Lights* gLastLights;

in Lights_Draw():
gLastLights = lights;
*/

typedef struct {
    u16 time;
    Color_RGB8 color;
} LightTimeMap;

#define SUN_MAP_COUNT 7
static const LightTimeMap sunMap[SUN_MAP_COUNT] = {
    {CLOCK_TIME(0, 0), {0, 0, 0}},
    {CLOCK_TIME(5, 40), {0, 0, 0}},
    {CLOCK_TIME(6, 0), {0xE0, 0x80, 0x80}},
    {CLOCK_TIME(7, 0), {0xFF, 0xF0, 0xE0}},
    {CLOCK_TIME(17, 0), {0xFF, 0xF0, 0xE0}},
    {CLOCK_TIME(18, 00), {0xFF, 0xE0, 0xB0}},
    {CLOCK_TIME(18, 20), {0, 0, 0}},
};
#define MOON_MAP_COUNT 7
static const LightTimeMap moonMap[MOON_MAP_COUNT] = {
    {CLOCK_TIME(0, 0), {0x60, 0x68, 0x70}},
    {CLOCK_TIME(5, 0), {0x60, 0x68, 0x70}},
    {CLOCK_TIME(5, 40), {0x40, 0x44, 0x48}},
    {CLOCK_TIME(6, 0), {0, 0, 0}},
    {CLOCK_TIME(17, 40), {0, 0, 0}},
    {CLOCK_TIME(18, 0), {0x40, 0x44, 0x48}},
    {CLOCK_TIME(19, 0), {0x60, 0x68, 0x70}},
};

Color_RGB8 GetSunMoonColor(bool isMoon){
    s32 count = isMoon ? MOON_MAP_COUNT : SUN_MAP_COUNT;
    const LightTimeMap* map = isMoon ? moonMap : sunMap;
    u16 t = gSaveContext.skyboxTime;
    for(s32 i=0; i<count-1; ++i){
        if(t < map[i+1].time){
            return BlendColors(map[i].color, map[i+1].color,
                (t - map[i].time) * 255 / (map[i+1].time - map[i].time));
        }
    }
    return map[count-1].color;
}

void Scene_DrawConfigWaterTempleBoss(PlayState* play) {
    // Island scene
    u32 gameplayFrames = play->gameplayFrames;
    s32 s1 = (gameplayFrames << 1) & 511;
    s32 t1 = (gameplayFrames >> 1) & 255;
    s32 s2 = ((gameplayFrames) + 87) & 511;
    s32 t2 = 255 - (((gameplayFrames) + 30) & 255);

    OPEN_DISPS(play->state.gfxCtx);
    
    Lights* lights = Graph_Alloc(play->state.gfxCtx, sizeof(Lights));
    bcopy(gLastLights, lights, sizeof(Lights));
    for(s32 i=0; i<lights->numLights; ++i){
        lights->l.l[i].l.size = 6;
    }
    Light* ambient = &lights->l.l[lights->numLights];
    Light* sun = &lights->l.l[lights->numLights - 1];
    Light* moon = &lights->l.l[lights->numLights - 2];
    
    Color_RGB8 skyColor = (Color_RGB8){0x80, 0x80, 0xFF};
    
    if(play->skyboxId == SKYBOX_NORMAL_SKY){
        const s32 zenithLine = 11;
        const s32 horizonLine = 37;
        // Textures are 128x64. Palettes are 256 entry, with one image's CIs
        // using the lower half and the other using the upper half.
        SkyboxContext* skyboxCtx = &play->skyboxCtx;
        Color_RGB8 zenith0  = SampleSky((u8*)skyboxCtx->staticSegments[0],
            &((*skyboxCtx->palettes)[0]), 128, zenithLine);
        Color_RGB8 horizon0 = SampleSky((u8*)skyboxCtx->staticSegments[0],
            &((*skyboxCtx->palettes)[0]), 128, horizonLine);
        Color_RGB8 zenith1  = SampleSky((u8*)skyboxCtx->staticSegments[1],
            &((*skyboxCtx->palettes)[0]), 128, zenithLine);
        Color_RGB8 horizon1 = SampleSky((u8*)skyboxCtx->staticSegments[1],
            &((*skyboxCtx->palettes)[0]), 128, horizonLine);
        Color_RGB8 zenith = BlendColors(
            zenith0, zenith1, play->envCtx.skyboxBlend);
        Color_RGB8 horizon = BlendColors(
            horizon0, horizon1, play->envCtx.skyboxBlend);
        skyColor = zenith;
        ambient->l.col[0] = ambient->l.colc[0] = horizon.r;
        ambient->l.col[1] = ambient->l.colc[1] = horizon.g;
        ambient->l.col[2] = ambient->l.colc[2] = horizon.b;
    }
    Color_RGB8 sunColor = GetSunMoonColor(false);
    Color_RGB8 moonColor = GetSunMoonColor(true);
    sun->l.col[0] = sun->l.colc[0] = sunColor.r;
    sun->l.col[1] = sun->l.colc[1] = sunColor.g;
    sun->l.col[2] = sun->l.colc[2] = sunColor.b;
    moon->l.col[0] = moon->l.colc[0] = moonColor.r;
    moon->l.col[1] = moon->l.colc[1] = moonColor.g;
    moon->l.col[2] = moon->l.colc[2] = moonColor.b;

    Gfx* displayList = Graph_Alloc(play->state.gfxCtx, 5 * sizeof(Gfx));
    gDPSetTileSize(displayList + 0, 0, s1, t1, s1 + 508, t1 + 252);
    gDPSetTileSize(displayList + 1, 1, s2, t2, s2 + 508, t2 + 252);
    gDPSetPrimColor(displayList + 2, 0, 0, skyColor.r, skyColor.g, skyColor.b, 0x40);
    gSPSetLights(displayList + 3, lights->numLights, lights->l);
    gSPEndDisplayList(displayList + 4);
    
    gSPSegment(POLY_XLU_DISP++, 0x08, displayList);

    CLOSE_DISPS(play->state.gfxCtx);
}
