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

    SetMode(MODE_0 | OBJ_ON | BG0_ON | OBJ_1D_MAP);

    dmaCopy(sheetPal, SPRITE_PALETTE, sheetPalLen);
    dmaCopy(sheetTiles, SPRITE_GFX, sheetTilesLen);

    for(int i = 0; i < 128; i++)
    {
        OAM[i].attr0 = ATTR0_DISABLED;
        OAM[i].attr1 = 0;
        OAM[i].attr2 = 0;
    }

    int x = 96;
    int y = 64;

    int upIndex = 0;
    int downIndex = 4;
    int leftIndex = 16;
    int rightIndex = 20;
    int bodyIndex = 32;
    int pumpkinIndex = 36;
    int bobIndex = 48;

    int movingDirection = 2;
    int currentSpriteIndex = leftIndex;

    int timer = 0;

    initBackground();
    
    while (1)
    {
        VBlankIntrWait();
        scanKeys();

        timer++;

        if (keysHeld() & KEY_UP)    movingDirection = 0;
        if (keysHeld() & KEY_DOWN)  movingDirection = 1;
        if (keysHeld() & KEY_LEFT)  movingDirection = 2;
        if (keysHeld() & KEY_RIGHT) movingDirection = 3;

        if (timer % 10 == 0)
        {
        
            switch (movingDirection)
            {
                case 0:
                    currentSpriteIndex = upIndex;
                    y -= 16;
                    break;

                case 1:
                    currentSpriteIndex = downIndex;
                    y += 16;
                    break;
                
                case 2:
                    currentSpriteIndex = leftIndex;
                    x -= 16;
                    break;

                case 3:
                    currentSpriteIndex = rightIndex;
                    x += 16;
                    break;
                
                default:
                    currentSpriteIndex = upIndex;
                    break;
            }    
        }

        // Player
        OAM[0].attr0 = (y & 0xFF) | ATTR0_SQUARE | ATTR0_COLOR_16;
        OAM[0].attr1 = (x & 0x1FF) | ATTR1_SIZE_16;
        OAM[0].attr2 = currentSpriteIndex | ATTR2_PALETTE(0);

        // Bob
        OAM[1].attr0 = (80 & 0xFF) | ATTR0_SQUARE | ATTR0_COLOR_16;
        OAM[1].attr1 = (144 & 0x1FF) | ATTR1_SIZE_16;
        OAM[1].attr2 = bobIndex | ATTR2_PALETTE(0);
    }
}