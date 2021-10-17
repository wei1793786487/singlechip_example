
#include <REG51.H>

sbit ENLED = P1 ^ 4;
sbit ADDR3 = P1 ^ 3;
sbit ADDR0 = P1 ^ 0;
sbit ADDR1 = P1 ^ 1;
sbit ADDR2 = P1 ^ 2;

//注意 这里老师的实验报告可能出现问题,应该是到第15个字符也就是的ledChar数组的最后一个 如果展示两位数就不算是静态显示了
unsigned char code ledChar[] = {
    0xc0, 0xF9, 0xa4, 0xB0, 0x99, 0x92, 0x82, 0xF8,
    0x80, 0x90, 0x88, 0x83, 0xc6, 0xA1, 0x86, 0x8e};

unsigned int index = 0;

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
        P0 = ledChar[index];
        index++;
        sleep(2000);
        if (index >= sizeof(ledChar))
        {
            index = 0;
        }
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
