
#include <REG51.H>

sbit ENLED = P1 ^ 4;
sbit ADDR3 = P1 ^ 3;

sbit Buzzer = P1 ^ 6;
sbit ADDR0 = P1 ^ 0;
sbit ADDR1 = P1 ^ 1;
sbit ADDR2 = P1 ^ 2;

sbit led = P0 ^ 0;

sbit key1 = P2 ^ 7;



void init();

void main()
{
    init();
    ADDR0 = 0;
    ADDR1 = 1;
    ADDR2 = 1;
    EA=1;
    TMOD = 0x01;
    TH0 = 0XfC;
    TL0 = 0x67;
    ET0 = 1;
    TR0 = 1;
    while (1)
    {
        led = key1;
        
    }
}

void init()
{
    ENLED = 0;
    ADDR3 = 1;
    P2 = 0xf7;
}

void InterruptTimer0() interrupt 1
{
    
    TH0 = 0XfC;
    TL0 = 0x67;
    if (key1==0)
    {
      Buzzer=~Buzzer;  
    }

}
