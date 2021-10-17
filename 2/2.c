
#include <REG51.H>

sbit ENLED = P1 ^ 4;
sbit ADDR3 = P1 ^ 3;
sbit ADDR0 = P1 ^ 0;
sbit ADDR1 = P1 ^ 1;
sbit ADDR2 = P1 ^ 2;

unsigned char code ledChar[] = {
    0xC0, 0xF9, 0xa4, 0xB0, 0x99, 0x92, 0x82, 0xF8,
    0x80, 0x90, 0x88, 0x83, 0xc6, 0xA1, 0x86, 0x8E};

unsigned int index = 15;

void init();

void sleep(time);

void main()
{
    init();
    ADDR0 = 0;
    ADDR1 = 0;
    ADDR2 = 0;
    while (1)
    {
        if (index == 0)
        {
            index = sizeof(ledChar);
        }
        P0 = ledChar[index];
        index--;
        sleep(10000);
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
    TH1 = 0xFC;
    TL1 = 0x64;
    TR1 = 1;
    while (end == 0)
    {
        if (TF1 == 1)
        {

            TF1 = 0;
            TH1 = 0xFC;
            TL1 = 0x64;
            cnt++;
            if (cnt >= time)
            {
                cnt = 0;
                end = 1;
            }
        }
    }
}
