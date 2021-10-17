
#include <REG51.H>

sbit ENLED = P1 ^ 4;
sbit ADDR3 = P1 ^ 3;
sbit ADDR0 = P1 ^ 0;
sbit ADDR1 = P1 ^ 1;
sbit ADDR2 = P1 ^ 2;

unsigned char code ledChar[] = {
    0xC0, 0xF9, 0xa4, 0xB0, 0x99, 0x92, 0x82, 0xF8,
    0x80};
void sleep(time);
void init();

void main()
{
    init();

    while (1)
    {

        sleep(1); //防止过快刷新
        
        P0 = ledChar[2];
        ADDR2 = 1;
        ADDR1 = 0;
        ADDR0 = 1;

        sleep(1);
        P0 = ledChar[0];
        ADDR2 = 1;
        ADDR1 = 0;
        ADDR0 = 0;

        sleep(1);
        P0 = ledChar[4];
        ADDR2 = 0;
        ADDR1 = 1;
        ADDR0 = 1;

        sleep(1);
        P0 = ledChar[1];
        ADDR2 = 0;
        ADDR1 = 1;
        ADDR0 = 0;

        sleep(1);
        P0 = ledChar[1];
        ADDR2 = 0;
        ADDR1 = 0;
        ADDR0 = 1;

        sleep(1);
        P0 = ledChar[8];
        ADDR2 = 0;
        ADDR1 = 0;
        ADDR0 = 0;
    }
}

void init()
{
    ENLED = 0;
    ADDR3 = 1;
}

void sleep(time)
{
    unsigned int cnt = 0;
    unsigned char end = 0;
    TMOD = 0x01;
    TH0 = 0xFC;
    TL0 = 0x64;
    TR0 = 1;
    while (end == 0)
    {
        if (TF0 == 1)
        {

            TF0 = 0;
            TH0 = 0xFC;
            TL0 = 0x64;
            cnt++;
            if (cnt >= time)
            {
                cnt = 0;
                end = 1;
            }
        }
    }
}
