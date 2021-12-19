
#include <REG51.H>

sbit ENLED = P1 ^ 4;
sbit ADDR3 = P1 ^ 3;


sbit ADDR0 = P1 ^ 0;
sbit ADDR1 = P1 ^ 1;
sbit ADDR2 = P1 ^ 2;

sbit led  =P0^6;

sbit key1 =P2^5;




void init();


void main()
{
    init();
    ADDR0 = 0;
    ADDR1 =1;
    ADDR2 = 1;
    while (1)
    {
        led=key1;
    }
}

void init()
{
    ENLED = 0;
    ADDR3 = 1;
    P2=0xf7;

}


