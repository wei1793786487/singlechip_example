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

void TurnMotor(unsigned long angle);
void delay();

sbit Buzzer = P1 ^ 6;
//蜂鸣器是否响
bit start;
void ConfigUART(unsigned int baud);

void main()
{
    EA = 1;

    ConfigUART(9600);
    TurnMotor(360);
    start = 1;

    while (1)
    {
    if (start==1)
    {
        Buzzer = ~Buzzer;
       delay();
    }
    if (SBUF==0xEE)
    {
      start=1;
    }else if (SBUF==0x01)
    {
      start=0;
    }

    }
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

void InterruptUART() interrupt 4
{
	if (RI)
	{
		RI=0;
		
	}
	if (T1)
	{
		T1=0;
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

