

#include <REG52.h>

sbit ADDR0 = P1 ^ 0;
sbit ADDR1 = P1 ^ 1;
sbit ADDR2 = P1 ^ 2;
sbit ADDR3 = P1 ^ 3;
sbit ENLED = P1 ^ 4;

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

unsigned int i;

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
    static unsigned char index = 0;
    //这里一定要是静态的 不然的话会每次触发中断的时候都报错
    static unsigned int cnt = 0;

    TH0 = 0xFC;
    TL0 = 0x67;
    P0 = 0xFF;

    cnt++;
    if (cnt > 1000)
    {
        cnt = 0;
        index++;
        if (index >= 8)
        {
            index = 0;
        }
    }

    switch (i)
    {
    case 0:
        ADDR2 = 0;
        ADDR1 = 0;
        ADDR0 = 0;
        P0 = ledChar[index][0];
        i++;
        break;
    case 1:
        ADDR2 = 0;
        ADDR1 = 0;
        ADDR0 = 1;
        P0 = ledChar[index][1];
        i++;
        break;
    case 2:
        ADDR2 = 0;
        ADDR1 = 1;
        ADDR0 = 0;
        P0 = ledChar[index][2];
        i++;
        break;
    case 3:
        ADDR2 = 0;
        ADDR1 = 1;
        ADDR0 = 1;
        P0 = ledChar[index][3];
        i++;
        break;
    case 4:
        ADDR2 = 1;
        ADDR1 = 0;
        ADDR0 = 0;
        P0 = ledChar[index][4];
        i++;
        break;
    case 5:
        ADDR2 = 1;
        ADDR1 = 0;
        ADDR0 = 1;
        P0 = ledChar[index][5];
        i++;
        break;
    case 6:
        ADDR2 = 1;
        ADDR1 = 1;
        ADDR0 = 0;
        P0 = ledChar[index][6];
        i++;
        break;
    case 7:
        ADDR2 = 1;
        ADDR1 = 1;
        ADDR0 = 1;
        P0 = ledChar[index][7];
        i = 0;
        break;

    default:
        break;
    }
}