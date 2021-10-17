

#include <REG52.h>

sbit ADDR0 = P1 ^ 0;
sbit ADDR1 = P1 ^ 1;
sbit ADDR2 = P1 ^ 2;
sbit ADDR3 = P1 ^ 3;
sbit ENLED = P1 ^ 4;

unsigned char ledChar[] = {
    0xfe,
    0xfc,
    0xf8,
    0xf0,
    0xe0,
    0xc0,
    0x80,
    0x00,
};

unsigned char flagls = 0;
unsigned char frist;
unsigned int i ;
unsigned int cnt = 0;

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

        
        if (flagls == 1)
        {
            flagls = 0;
            //取出来第一个数组;
            frist = ledChar[0];
            for (i = 0; i < sizeof(ledChar); i++)
            {
                ledChar[i] = ledChar[i+1];
            }
            //把第一个数组放到最后一位
            ledChar[sizeof(ledChar)-1] = frist;
        }
    }
}

void InterruptTimer0() interrupt 1
{
    static unsigned char i = 0;
    TH0 = 0xFC;
    TL0 = 0x67;
    P0 = 0xFF;
    cnt++;
    if (cnt > 1000)
    {

        cnt = 0;
        flagls = 1;
    }
    switch (i)
    {
    case 0:
        ADDR2 = 0;
        ADDR1 = 0;
        ADDR0 = 0;
        P0 = ledChar[i];
        i++;
        break;
    case 1:
        ADDR2 = 0;
        ADDR1 = 0;
        ADDR0 = 1;
        P0 = ledChar[i];
        i++;
        break;
    case 2:
        ADDR2 = 0;
        ADDR1 = 1;
        ADDR0 = 0;
        P0 = ledChar[i];
        i++;
        break;
    case 3:
        ADDR2 = 0;
        ADDR1 = 1;
        ADDR0 = 1;
        P0 = ledChar[i];
        i++;
        break;
    case 4:
        ADDR2 = 1;
        ADDR1 = 0;
        ADDR0 = 0;
        P0 = ledChar[i];
        i++;
        break;
    case 5:
        ADDR2 = 1;
        ADDR1 = 0;
        ADDR0 = 1;
        P0 = ledChar[i];
        i++;
        break;
    case 6:
        ADDR2 = 1;
        ADDR1 = 1;
        ADDR0 = 0;
        P0 = ledChar[i];
        i++;
        break;
    case 7:
        ADDR2 = 1;
        ADDR1 = 1;
        ADDR0 = 1;
        P0 = ledChar[i];
        i = 0;
        break;

    default:
        break;
    }
}