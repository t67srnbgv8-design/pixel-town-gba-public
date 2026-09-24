#include <gba.h>
#include <stdio.h>

int main(void) {
    irqInit();
    irqEnable(IRQ_VBLANK);
    consoleDemoInit();

    iprintf("\x1b[2J");
    iprintf("\n        PIXEL TOWN\n");
    iprintf("       GBA BUILD TEST\n\n");
    iprintf("  Steuerkreuz: bewegen\n");
    iprintf("  Delta/GBA kompatibel\n\n");

    int x = 15;
    int y = 12;

    while (1) {
        VBlankIntrWait();
        scanKeys();

        int oldx = x;
        int oldy = y;
        u16 keys = keysHeld();

        if ((keys & KEY_LEFT) && x > 1) x--;
        if ((keys & KEY_RIGHT) && x < 28) x++;
        if ((keys & KEY_UP) && y > 7) y--;
        if ((keys & KEY_DOWN) && y < 18) y++;

        if (oldx != x || oldy != y)
            iprintf("\x1b[%d;%dH ", oldy, oldx);

        iprintf("\x1b[%d;%dH@", y, x);
    }
}
