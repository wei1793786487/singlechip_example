

#include <REG52.H>

sbit ENLED = P1 ^ 4;
sbit ADDR3 = P1 ^ 3;

sbit ADDR0 = P1 ^ 0;
sbit ADDR1 = P1 ^ 1;
sbit ADDR2 = P1 ^ 2;

sbit key1 = P2 ^ 4;

sbit led = P0 ^ 0;

unsigned int enter = 0;

bit KeySta = 1;

//注意防抖的处理  按下按钮间隔过短会导致失效
void main()
{

    bit backUp = 1;
    unsigned char cnt = 0;

    EA = 1;
    ENLED = 0;
    ADDR3 = 1;
    ADDR0 = 0;
    ADDR1 = 1;
    ADDR2 = 1;

    TMOD = 0x01;
    TH0 = 0XfC;
    TL0 = 0x67;
    ET0 = 1;
    TR0 = 1;
    P2 = 0xf7;

    while (1)
    {
        if (KeySta != backUp)
        {
            if (backUp == 0)
            {
                enter++;
                if (enter % 2 == 0)
                {
                    led = 1;
                }
                else
                {
                    led = 0;
                }
            }
        }
        backUp = KeySta;
    }
}

void InterruptTimer0() interrupt 1
{
    static unsigned char keyBuf = 0xff;

    TH0 = 0XfC;
    TL0 = 0x67;

    keyBuf = (keyBuf << 1) | key1;

    if (keyBuf == 0x00)
    {
        KeySta = 0;
    }
    else if (keyBuf == 0xff)
    {
        KeySta = 1;
    }
    else
    {
    }
}
