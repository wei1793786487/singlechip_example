#include <REG52.H>

unsigned char BeatCode[8] = {
    0x6,
    0x7,
    0x3,
    0xb,
    0x9,
    0xD,
    0xC,
    0xE,
};

void sleep(time);
void delay();
void TurnMotor(unsigned long angle);

void main()
{

    while (1)
    {
        /* code */
        TurnMotor(90);
        sleep(30000);
    };
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

