

#include <REG52.h>

sbit ADDR0 = P1 ^ 0;
sbit ADDR1 = P1 ^ 1;
sbit ADDR2 = P1 ^ 2;
sbit ADDR3 = P1 ^ 3;
sbit ENLED = P1 ^ 4;

unsigned char code ledChar[] = {
    0xc0, 0xF9, 0xa4, 0xB0, 0x99, 0x92, 0x82, 0xF8,
    0x80, 0x90, 0x88, 0x83, 0xc6, 0xA1, 0x86, 0x8e};

unsigned char ledBuff[6] = {
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

unsigned char i = 0;

unsigned int cnt = 0;

unsigned char flagls= 0;

void main()
{

    unsigned long sec = 999999;
    EA = 1;
    ENLED = 0;
    ADDR3 = 1;
    TMOD = 0x01;
    TH0 = 0xFC;
    TL0 = 0x67;
    ET0 = 1;
    TR0 = 1;
    while (1)
    {
        if (flagls == 1)
        {
            flagls = 0;
            sec=sec-3;
            ledBuff[0] = ledChar[sec % 10];
            ledBuff[1] = ledChar[sec / 10 % 10];
            ledBuff[2] = ledChar[sec / 100 % 10];
            ledBuff[3] = ledChar[sec / 1000 % 10];
            ledBuff[4] = ledChar[sec / 10000 % 10];
            ledBuff[5] = ledChar[sec / 100000 % 10];
        }
    }
}

void InterruptTimer0() interrupt 1
{
    TH0 = 0xFC;
    TL0 - 0x67;
    cnt++;
    if (cnt > 2000)
    {
        cnt = 0;
        flagls = 1;
    }
    P0 = 0xff;
    switch (i)
    {
    case 0:
        ADDR2 = 0;
        ADDR1 = 0;
        ADDR0 = 0;
        i++;
        P0 = ledBuff[0];
        break;
    case 1:
        ADDR2 = 0;
        ADDR1 = 0;
        ADDR0 = 1;
        i++;
        P0 = ledBuff[1];
        break;
    case 2:
        ADDR2 = 0;
        ADDR1 = 1;
        ADDR0 = 0;
        i++;
        P0 = ledBuff[2];
        break;
    case 3:
        ADDR2 = 0;
        ADDR1 = 1;
        ADDR0 = 1;
        i++;
        P0 = ledBuff[3];
        break;
    case 4:
        ADDR2 = 1;
        ADDR1 = 0;
        ADDR0 = 0;
        i++;
        P0 = ledBuff[4];
        break;
    case 5:
        ADDR2 = 1;
        ADDR1 = 0;
        ADDR0 = 1;
        i=0;
        P0 = ledBuff[5];
        break;

    default:
        break;
    }
}