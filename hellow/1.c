#include <reg52.h>

unsigned char table[] = {0xc0, 0xf9, 0xa4, 0xb0, 0x99, 0x92, 0x82, 0xf8, 0x80, 0x90, 0x88, 0x83, 0xc6, 0xa1, 0x86, 0x8e};
int count = 0, i = 0;
void main()
{
    TMOD = 0X01;
    TH0 = 0XFC;
    TL0 = 0X67;
    ET0 = 1;
    TR0 = 1;
    EA = 1;

    P0 = 0xff;
    while (1)
    {
    }
}
void wulala() interrupt 1
{

    TH0 = 0XFC;
    TL0 = 0X67;
    if (count++ > 1000)
    {
        if (i++ > 8)
            i = 0;
        count = 0;
    }
    P0 = 0xff;
     P0 = table[i];
}