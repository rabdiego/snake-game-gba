
#include <gba_console.h>
#include <gba_video.h>
#include <gba_interrupt.h>
#include <gba_systemcalls.h>
#include <gba_input.h>
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    REG_DISPCNT = MODE_3 | BG2_ENABLE;

    int x = 100, y = 60;

    while (1)
	{
        scanKeys();
        u16 keys = keysHeld();

        if (keys & KEY_UP) y--;
        if (keys & KEY_DOWN) y++;
        if (keys & KEY_LEFT) x--;
        if (keys & KEY_RIGHT) x++;

        for (int i = 0; i < 240*160; i++)
		{
			((u16*)VRAM)[i] = RGB5(0, 0, 10);
		}

        for (int i = 0; i < 10; i++)
		{
			for (int j = 0; j < 10; j++)
			{
				((u16*)VRAM)[(y + i)*240 + (x + j)] = RGB5(31, 0, 0);
			}
		} 
    }
}

