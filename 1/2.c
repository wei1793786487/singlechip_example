
#include <REG51.H>

sbit LED = P0 ^ 0;
sbit ADDR0 = P1 ^ 0;
sbit ADDR1 = P1 ^ 1;
sbit ADDR2 = P1 ^ 2;
sbit ADDR3 = P1 ^ 3;
sbit ENLED = P1 ^ 4;


void init();
void sleep(time);
void main()
{
    init();
    while (1)
    { 
      sleep(1000);
      LED=0;
      sleep(1000);
      LED=1;


      
    }
}

void init()
{
    ENLED = 0;
    ADDR3 = 1;
    ADDR2 = 1;
    ADDR1 = 1;
    ADDR0 = 0;
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
