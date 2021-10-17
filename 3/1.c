

#include <REG52.h>

sbit ADDR0 = P1 ^ 0;
sbit ADDR1 = P1 ^ 1;
sbit ADDR2 = P1 ^ 2;
sbit ADDR3 = P1 ^ 3;
sbit ENLED = P1 ^ 4;

unsigned char code ledChar[] = {
    0xfe,
    0xfc,
    0xf8,
    0xf0,
    0xe0,
    0xc0,
    0x80,
    0x00,
};
void main()
{
    EA = 1;
    ENLED = 0;
    ADDR3 = 0;
    TMOD = 0x01;
    TH0 = 0xFC;
    TL0 = 0x67;
    ET0 = 1;
    TR0 = 1;
    while (1)
    {
    }
}

void InterruptTimer0() interrupt 1
{
    static unsigned char i = 0;
    TH0 = 0xFC;
    TL0 = 0x67;
    P0 = 0xFF;
    switch (i)
    {
    case 0:
        ADDR2 = 0;
        ADDR1 = 0;
        ADDR0 = 0;
        i++;
        P0 = ledChar[i];
        break;
    case 1:
        ADDR2 = 0;
        ADDR1 = 0;
        ADDR0 = 1;
        i++;
        P0 = ledChar[i];
        break;
    case 2:
        ADDR2 = 0;
        ADDR1 = 1;
        ADDR0 = 0;
        i++;
        P0 = ledChar[i];
        break;
    case 3:
        ADDR2 = 0;
        ADDR1 = 1;
        ADDR0 = 1;
        i++;
        P0 = ledChar[i];
        break;
    case 4:
        ADDR2 = 1;
        ADDR1 = 0;
        ADDR0 = 0;
        i++;
        P0 = ledChar[i];
        break;
    case 5:
        ADDR2 = 1;
        ADDR1 = 0;
        ADDR0 = 1;
        i++;
        P0 = ledChar[i];
        break;
    case 6:
        ADDR2 = 1;
        ADDR1 = 1;
        ADDR0 = 0;
        i++;
        P0 = ledChar[i];
        break;
    case 7:
        ADDR2 = 1;
        ADDR1 = 1;
        ADDR0 = 1;
        i = 0;
        P0 = ledChar[i];
        break;

    default:
        break;
    }
}