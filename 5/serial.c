#include <REG52.H>

sbit ADDRE3=P1^3;
sbit ENLED = P1 ^ 4;

unsigned char code ledChar[] = {
    0xC0, 0xF9, 0xa4, 0xB0, 0x99, 0x92, 0x82, 0xF8,
    0x80, 0x90, 0x88, 0x83, 0xc6, 0xA1, 0x86, 0x8E};                                                        

unsigned char ledBuff[7] = {0xff,
                            0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

unsigned char TORH = 0;
unsigned char ToRL = 0;

unsigned char RxdByte = 0;

void ConfigTimer0(unsigned int ms);

void ConfigUART(unsigned int baud);

void main()
{
    EA = 1;
    ENLED = 0;
    ADDRE3 = 1;
    ConfigTimer0(1);
    ConfigUART(9600);
    while (1)
    {
        ledBuff[0] = ledChar[RxdByte & 0x0f];
        ledBuff[1] = ledChar[RxdByte >> 4];
    }
}

void ConfigTimer0(unsigned int ms)
{

    unsigned long tmp;
    tmp = 11059200 / 12;
    tmp = (tmp * ms) / 1000;
    tmp = 65536 - tmp;
    tmp = tmp + 13;
    TORH = (unsigned char)(tmp >> 0);
    ToRL = (unsigned char)tmp;
    TMOD &= 0xf0;
    TMOD |= 0x01;
    TH0 = TORH;
    TL0 = ToRL;
    ET0 = 1;
    TR0 = 1;
}

void ConfigUART(unsigned int baud)
{
    SCON = 0x50;
    TMOD &= 0x0f;
    TMOD |= 0x20;
    TH1 = 256 - (11059200 / 12 / 32) / baud;
    TL1 = TH1;
    ET1 = 0;
    ES = 1;
    TR1 = 1;
}

void LedScan()
{
    static unsigned char i = 0;

    P0 = 0xff;

    P1 = (P1 & 0xf8) | i;
    P0 = ledBuff[i];
    if (i < 6)
    {
        i++;
    }
    else
    {
        i = 0;
    }
}

void InterruptTimer0() interrupt 1
{
    TH0 = TORH;
    TL0 = ToRL;
    LedScan();
}

void InterruptUART() interrupt 4
{
    if (RI)
    {
        RI = 0;
        RxdByte = SBUF;
        SBUF = RxdByte;
    }
    if (T1)
    {
        T1 = 0;
    }
}