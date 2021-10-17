#include <reg52.h>

sbit ADDR0 = P1 ^ 0;
sbit ADDR1 = P1 ^ 1;
sbit ADDR2 = P1 ^ 2;
sbit ADDR3 = P1 ^ 3;
sbit ENLED = P1 ^ 4;

sbit KEY_IN_1 = P2 ^ 4;
sbit KEY_IN_2 = P2 ^ 5;
sbit KEY_IN_3 = P2 ^ 6;
sbit KEY_IN_4 = P2 ^ 7;
sbit KEY_OUT_1 = P2 ^ 3;
sbit KEY_OUT_2 = P2 ^ 2;
sbit KEY_OUT_3 = P2 ^ 1;
sbit KEY_OUT_4 = P2 ^ 0;

unsigned char code ledChar[8][8] = {
    {0xfe, 0xfc, 0xf8, 0xf0, 0xe0, 0xc0, 0x80, 0x00},
    {0x7f, 0x7e, 0x7c, 0x78, 0x70, 0x60, 0x40, 0x00},
    {0xbf, 0x3f, 0x3e, 0x3c, 0x38, 0x30, 0x20, 0x00},
    {0xdf, 0x9f, 0x1f, 0x1e, 0x1c, 0x18, 0x10, 0x00},
    {0xef, 0xcf, 0x8f, 0x0f, 0x1f, 0x0c, 0x08, 0x00},
    {0xf7, 0xe7, 0xc7, 0x87, 0x07, 0x06, 0x04, 0x00},
    {0xfb, 0xf3, 0xe3, 0xc3, 0x83, 0x03, 0x02, 0x00},
    {0xfd, 0xf9, 0xf1, 0xe1, 0xc1, 0x81, 0x01, 0x00},
};

unsigned char KeySta[4][4] = {
    {1, 1, 1, 1}, {1, 1, 1, 1}, {1, 1, 1, 1}, {1, 1, 1, 1}};

void move(direction);

void main()
{
    unsigned char i, j;
    unsigned char backup[4][4] = {
        {1, 1, 1, 1}, {1, 1, 1, 1}, {1, 1, 1, 1}, {1, 1, 1, 1}};

    ENLED = 0;
    ADDR3 = 1;
    ADDR2 = 0;
    ADDR1 = 0;
    ADDR0 = 0;

    EA = 1;
    TMOD = 0x01;
    TH0 = 0xFC;
    TL0 = 0x67;
    ET0 = 1;
    TR0 = 1;

    while (1)
    {
        for (i = 0; i < 4; i++)
        {
            for (j = 0; j < 4; j++)
            {
                if (backup[i][j] != KeySta[i][j])
                {
                    if (backup[i][j] != 0)
                    {
                        //有switch我就不用我就喜欢用if
                        if (i * 4 + j == 4)
                        {
                            move(0);
                        }
                        else if (i * 4 + j == 12)
                        {
                            move(1);
                        }

                        else if (i * 4 + j == 8)
                        {
                            move(2);
                        }
                        else if (i * 4 + j == 16)
                        {
                            move(3);
                        }
                        else
                        {
                        }
                    }
                    backup[i][j] = KeySta[i][j];
                }
            }
        }
    }
}

void InterruptTimer0() interrupt 1
{

    unsigned char i;
    static unsigned char keyout = 0;
    static unsigned char keybuf[4][4] = {
        {0xFF, 0xFF, 0xFF, 0xFF}, {0xFF, 0xFF, 0xFF, 0xFF}, {0xFF, 0xFF, 0xFF, 0xFF}, {0xFF, 0xFF, 0xFF, 0xFF}};

    TH0 = 0xFC;
    TL0 = 0x67;

    keybuf[keyout][0] = (keybuf[keyout][0] << 1) | KEY_IN_1;
    keybuf[keyout][1] = (keybuf[keyout][1] << 1) | KEY_IN_2;
    keybuf[keyout][2] = (keybuf[keyout][2] << 1) | KEY_IN_3;
    keybuf[keyout][3] = (keybuf[keyout][3] << 1) | KEY_IN_4;

    for (i = 0; i < 4; i++)
    {
        if ((keybuf[keyout][i] & 0x0F) == 0x00)
        {
            KeySta[keyout][i] = 0;
        }
        else if ((keybuf[keyout][i] & 0x0F) == 0x0F)
        {
            KeySta[keyout][i] = 1;
        }
    }

    keyout++;
    keyout = keyout & 0x03;
    switch (keyout)
    {
    case 0:
        KEY_OUT_4 = 1;
        KEY_OUT_1 = 0;
        break;
    case 1:
        KEY_OUT_1 = 1;
        KEY_OUT_2 = 0;
        break;
    case 2:
        KEY_OUT_2 = 1;
        KEY_OUT_3 = 0;
        break;
    case 3:
        KEY_OUT_3 = 1;
        KEY_OUT_4 = 0;
        break;
    default:
        break;
    }
}

void move(direction)
{
    // 0  为上   1为下  左为2  右为3
}