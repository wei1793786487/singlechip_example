#include <reg52.h>
sbit ADDR0 = P1^0;
sbit ADDR1 = P1^1;
sbit ADDR2 = P1^2;
sbit ADDR3 = P1^3;
sbit ENLED = P1^4;
sbit KEY1 = P2^4;
sbit KEY2 = P2^5;
sbit KEY3 = P2^6;
sbit KEY4 = P2^7;
sbit KEY5 = 
unsigned char code LedChar[] = { //数码管显示字符转换表
0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8,
0x80, 0x90, 0x88, 0x83, 0xC6, 0xA1, 0x86, 0x8E
};
void main()
{
    bit backup = 1; //定义一个位变量，保存前一次扫描的按键值
    unsigned char cnt = 0; //定义一个计数变量，记录按键按下的次数
    ENLED = 0; //选择数码管DS1 进行显示
    ADDR3 = 1;
    ADDR2 = 0;
    ADDR1 = 0;
    ADDR0 = 0;
    P2 = 0xFb; //P2.2 置0，即KeyOut2 输出低电平
    P0 = LedChar[cnt]; //显示按键次数初值
    while (1)
        {
         if (KEY5 != backup) //当前值与前次值不相等说明此时按键有动作
        {
         if (backup == 0) //如果前次值为0，则说明当前是由0 变1，即按键弹起
        {
          cnt++; //按键次数+1
         if (cnt >= 10)
        { //只用1 个数码管显示，所以加到10 就清零重新开始
         cnt = 0;
        }
         P0 = LedChar[cnt]; //计数值显示到数码管上
        }
            backup = KEY5; //更新备份为当前值，以备进行下次比较
        }
    }
}