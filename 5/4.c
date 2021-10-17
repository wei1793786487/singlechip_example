

#include <REG52.h>

unsigned char BeatCode[8] = {
     0xE,
    0xC,
    0xD,
    0x9,
    0xb,
    0x3,
    0x7,
    0x6,
};

unsigned int i;

sbit Buzzer = P1 ^ 6;

//蜂鸣器是否响
bit start = 0;

void TurnMotor(unsigned long angle);
void delay();
void main()
{
    EA = 1;

    TMOD = 0x01;
    TH0 = 0xFC;
    TL0 = 0x67;
    ET0 = 1;
    TR0 = 1;

    while (1)
    {
    }
}

void InterruptTimer0() interrupt 1
{

    //用来判断记录1秒的cnt
    static unsigned int cnt = 0;

    //用来判断记录2秒的cnt
    static unsigned int cnt_2 = 0;

    TH0 = 0xFC;
    TL0 = 0x67;
    P0 = 0xFF;

    cnt++;
    cnt_2++;

    if (cnt_2 > 2000)
    {
        TurnMotor(30);
        cnt_2 = 0;
    }

    if (cnt > 1000)
    {
        Buzzer = ~Buzzer;
        cnt = 0;
        delay();
        Buzzer = ~Buzzer;
    }
}

void TurnMotor(unsigned long angle)
{
    unsigned char tmp;
    unsigned char index = 0;
    //这个电机需要64乘64个才能转一圈,与360相除是因为要求出转1度的所需的转的次数
    unsigned long beats = (angle * 4096) / 360;

    while (beats--)
    {
        tmp = P1;
        //清零低四位.因为前面的p的的高四位是不属于p的
        tmp = tmp & 0xf0;
        //这里是通过或运算来把低四位赋值给控制的地方
        tmp = tmp | BeatCode[index];
        //重新给p赋值
        P1 = tmp;
        index++;
        // index = index & 0x07; 这个更牛逼
        if (index == 8)
        {
            index = 0;
        }

        delay();
    }
    P1 = P1 | 0x0f;
}

void delay()
{
    unsigned int i = 200;
    while (i--)
    {
        /* code */
    };
}