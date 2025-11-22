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
#include <time.h>

#include "sheet.h"
#include "bg.h"

#define MAX_SNAKE_LENGTH 127

typedef struct
{
    u16 attr0;
    u16 attr1;
    u16 attr2;
    u16 pad;
} OBJ_ATTR;

typedef struct
{
    int x;
    int y;
    int direction;
} Position;

int generateRandomTilePos(int max)
{
    int range = max + 1;
    return (rand() % range) * 16;
}

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
    srand(time(NULL));

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

    int upIndex = 0;
    int downIndex = 4;
    int leftIndex = 16;
    int rightIndex = 20;
    int bodyIndex = 32;
    int pumpkinIndex = 36;
    int bobIndex = 48;

    int tailDirection = 2;
    int currentSpriteIndex = leftIndex;

    int timer = 0;

    Position body[MAX_SNAKE_LENGTH];
    int snakeLength = 1;
    body[0].x = 96; body[0].y = 64; body[0].direction = 2;

    Position bob;
    bob.x = 144; bob.y = 80; bob.direction = 0;

    initBackground();
    
    while (1)
    {
        VBlankIntrWait();
        scanKeys();

        timer++;

        if (keysHeld() & KEY_UP)    body[0].direction = 0;
        if (keysHeld() & KEY_DOWN)  body[0].direction = 1;
        if (keysHeld() & KEY_LEFT)  body[0].direction = 2;
        if (keysHeld() & KEY_RIGHT) body[0].direction = 3;
        
        tailDirection = body[snakeLength-1].direction;

        if (timer % 10 == 0)
        {
            for (int i = snakeLength - 1; i > 0; i--)
            {
                body[i].x = body[i-1].x;
                body[i].y = body[i-1].y;
                body[i].direction = body[i-1].direction;
            }
        
            switch (body[0].direction)
            {
                case 0:
                    currentSpriteIndex = upIndex;
                    body[0].y -= 16;
                    break;

                case 1:
                    currentSpriteIndex = downIndex;
                    body[0].y += 16;
                    break;
                
                case 2:
                    currentSpriteIndex = leftIndex;
                    body[0].x -= 16;
                    break;

                case 3:
                    currentSpriteIndex = rightIndex;
                    body[0].x += 16;
                    break;
                
                default:
                    break;
            }

            if (body[0].x == bob.x && body[0].y == bob.y)
            {
                switch (tailDirection)
                {
                    case 0:
                        body[snakeLength].x = body[snakeLength-1].x;
                        body[snakeLength].y = body[snakeLength-1].y + 16;
                        break;
                    
                    case 1:
                        body[snakeLength].x = body[snakeLength-1].x;
                        body[snakeLength].y = body[snakeLength-1].y - 16;
                        break;

                    case 2:
                        body[snakeLength].x = body[snakeLength-1].x + 16;
                        body[snakeLength].y = body[snakeLength-1].y;
                        break;

                    case 3:
                        body[snakeLength].x = body[snakeLength-1].x - 16;
                        body[snakeLength].y = body[snakeLength-1].y;
                        break;

                    default:
                        break;
                }
                
                body[snakeLength].direction = tailDirection;
                snakeLength++;

                int bobIsOutSnake = 0;
                while (!bobIsOutSnake)
                {
                    bob.x = generateRandomTilePos(14);
                    bob.y = generateRandomTilePos(9);

                    bobIsOutSnake = 1;
                    for (int i = 0; i < snakeLength; i++)
                    {
                        if (bob.x == body[i].x && bob.y == body[i].y)
                        {
                            bobIsOutSnake = 0;
                        }
                    }
                }
            }
        }

        // Player
        OAM[0].attr0 = (body[0].y & 0xFF) | ATTR0_SQUARE | ATTR0_COLOR_16;
        OAM[0].attr1 = (body[0].x & 0x1FF) | ATTR1_SIZE_16;
        OAM[0].attr2 = currentSpriteIndex | ATTR2_PALETTE(0);

        // Body
        for (int i = 1; i < snakeLength; i++)
        {
            OAM[i].attr0 = (body[i].y & 0xFF) | ATTR0_SQUARE | ATTR0_COLOR_16;
            OAM[i].attr1 = (body[i].x & 0x1FF) | ATTR1_SIZE_16;
            OAM[i].attr2 = bodyIndex | ATTR2_PALETTE(0);
        }

        // Bob
        OAM[127].attr0 = (bob.y & 0xFF) | ATTR0_SQUARE | ATTR0_COLOR_16;
        OAM[127].attr1 = (bob.x & 0x1FF) | ATTR1_SIZE_16;
        OAM[127].attr2 = bobIndex | ATTR2_PALETTE(0);
    }
}