#include <gba_console.h>
#include <gba_video.h>
#include <gba_interrupt.h>
#include <gba_systemcalls.h>
#include <gba_input.h>
#include <gba_sprites.h>
#include <gba_dma.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sheet.h"
#include "bg.h"

typedef struct
{
    u16 attr0;
    u16 attr1;
    u16 attr2;
    u16 pad;
} OBJ_ATTR;

void initBackground(void)
{
    REG_DISPCNT = MODE_0 | BG0_ON;
    REG_BG0CNT = BG_SIZE_0 | SCREEN_BASE(31) | CHAR_BASE(0);

    memcpy(CHAR_BASE_BLOCK(0), bgTiles, bgTilesLen);
    memcpy(BG_PALETTE, bgPal, bgPalLen);

    u16 *map = SCREEN_BASE_BLOCK(31);

    int patternWidth = 4;
    int patternHeight = 4;

    for(int y = 0; y < 32; y++)
    {
        for(int x = 0; x < 32; x++)
        {
            int patternX = x % patternWidth;
            int patternY = y % patternHeight;

            int mapIndex = patternY * 32 + patternX;
            
            map[y * 32 + x] = bgMap[mapIndex];
        }
    }
}

int main(void)
{
    irqInit();
    irqEnable(IRQ_VBLANK);

    SetMode(MODE_0 | OBJ_ON | OBJ_1D_MAP);

    dmaCopy(sheetPal, SPRITE_PALETTE, sheetPalLen);
    dmaCopy(sheetTiles, SPRITE_GFX, sheetTilesLen);

    int x = 100;
    int y = 60;

    initBackground();

    while (1)
    {
        VBlankIntrWait();
        
        scanKeys(); 

        OAM[0].attr0 = ATTR0_SQUARE | y;
        OAM[0].attr1 = ATTR1_SIZE_16 | x;
        OAM[0].attr2 = 0;

        if (keysHeld() & KEY_RIGHT) x++;
        if (keysHeld() & KEY_LEFT)  x--;
        if (keysHeld() & KEY_UP)    y--;
        if (keysHeld() & KEY_DOWN)  y++;
    }
}