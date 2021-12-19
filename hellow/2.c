

#include <REG52.h>



unsigned char code ledChar[] = {
    0xc0, 0xF9, 0xa4, 0xB0, 0x99, 0x92, 0x82, 0xF8,
    0x80};

unsigned char ledBuff[2] = {
    0xff, 0xff};

unsigned char i = 0;

unsigned int start = 0;

unsigned int cnt = 0;

unsigned char flagls = 0;

void main()
{

    unsigned long sec = 0;
  
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
            sec = sec + 2;

            if (sec > 999)
            {
                sec = 0;
            }
            ledBuff[0] = ledChar[sec % 10];
            ledBuff[1] = ledChar[sec / 10 % 10];
            ledBuff[1] = ledChar[sec / 100% 10];
        }
    }
}

void InterruptTimer0() interrupt 1
{
   
    TH0 = 0xFC;
    TL0 - 0x67;
    cnt++;
    if (cnt > 1000)
    {
        start = 1;
        cnt = 0;
        flagls = 1;
    }
    P0 = 0xff;
    switch (i)
    {
    case 0:
        P2=0x0E;
        i++;
        P0 = ledBuff[0];
        break;
    case 1:
        P2=0x1E;
        i++;
        P0 = ledBuff[1];
        break;
     case 2:
        P2=0x2E;   
        i = 0;
        P0 = ledBuff[2];
        break;
    default:
        break;
    }
}