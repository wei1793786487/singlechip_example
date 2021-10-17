
#include <reg52.h>

sbit ADDR0 = P1 ^ 0;
sbit ADDR1 = P1 ^ 1;
sbit ADDR2 = P1 ^ 2;
sbit ADDR3 = P1 ^ 3;
sbit ENLED = P1 ^ 4;

sbit KEY_IN_1 = P2 ^ 4;
sbit KEY_IN_2 = P2 ^ 5;
sbit KEY_IN_3 = P2 ^ 6;
sbit KEY_IN_4 = P2 ^ 7;
sbit KEY_OUT_1 = P2 ^ 3;
sbit KEY_OUT_2 = P2 ^ 2;
sbit KEY_OUT_3 = P2 ^ 1;
sbit KEY_OUT_4 = P2 ^ 0;

//这里的index是行的索引，所以下面只要定义一个列的索引就行了
//定义太多变量给单片机整的没内存的可还行
static signed char index = 0;

unsigned char frame_column_index = 0;

//保存左右位移的每一帧
static code unsigned char column[8] = {
    0x7f, 0xbf, 0xdf, 0xef, 0xf7, 0xfb, 0xfd, 0xfe};

    
unsigned char ledChar[8][8] = {
    {0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
    {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
    {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
    {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
    {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
    {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
    {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
    {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
};

unsigned char KeySta[4][4] = {
    {1, 1, 1, 1}, {1, 1, 1, 1}, {1, 1, 1, 1}, {1, 1, 1, 1}};

void move(direction);

void keyListen();

void latticeListen();

void main()
{
    unsigned char i, j;
    unsigned char backup[4][4] = {
        {1, 1, 1, 1}, {1, 1, 1, 1}, {1, 1, 1, 1}, {1, 1, 1, 1}};

    ENLED = 0;
    ADDR3 = 0;
    ADDR2 = 0;
    ADDR1 = 0;
    ADDR0 = 0;

    EA = 1;
    TMOD = 0x01;
    TH0 = 0xFC;
    TL0 = 0x67;
    ET0 = 1;
    TR0 = 1;

    while (1)
    {
        for (i = 0; i < 4; i++)
        {
            for (j = 0; j < 4; j++)
            {
                if (backup[i][j] != KeySta[i][j])
                {
                    if (backup[i][j] != 0)
                    {
                        if (i * 4 + j == 3)
                        {
                            move(0);
                        }
                        else if (i * 4 + j == 11)
                        {
                            move(1);
                        }

                        else if (i * 4 + j == 7)
                        {
                            move(2);
                        }
                        else if (i * 4 + j == 15)
                        {
                            move(3);
                        }
                        else
                        {
                        }
                    }
                    backup[i][j] = KeySta[i][j];
                }
            }
        }
    }
}

void InterruptTimer0() interrupt 1
{
    keyListen();
    latticeListen();
}

void keyListen()
{

    unsigned char i;
    static unsigned char keyout = 0;
    static unsigned char keybuf[4][4] = {
        {0xFF, 0xFF, 0xFF, 0xFF}, {0xFF, 0xFF, 0xFF, 0xFF}, {0xFF, 0xFF, 0xFF, 0xFF}, {0xFF, 0xFF, 0xFF, 0xFF}};

    TH0 = 0xFC;
    TL0 = 0x67;

    keybuf[keyout][0] = (keybuf[keyout][0] << 1) | KEY_IN_1;
    keybuf[keyout][1] = (keybuf[keyout][1] << 1) | KEY_IN_2;
    keybuf[keyout][2] = (keybuf[keyout][2] << 1) | KEY_IN_3;
    keybuf[keyout][3] = (keybuf[keyout][3] << 1) | KEY_IN_4;

    for (i = 0; i < 4; i++)
    {
        if ((keybuf[keyout][i] & 0x0F) == 0x00)
        {
            KeySta[keyout][i] = 0;
        }
        else if ((keybuf[keyout][i] & 0x0F) == 0x0F)
        {
            KeySta[keyout][i] = 1;
        }
    }

    keyout++;
    keyout = keyout & 0x03;
    switch (keyout)
    {
    case 0:
        KEY_OUT_4 = 1;
        KEY_OUT_1 = 0;
        break;
    case 1:
        KEY_OUT_1 = 1;
        KEY_OUT_2 = 0;
        break;
    case 2:
        KEY_OUT_2 = 1;
        KEY_OUT_3 = 0;
        break;
    case 3:
        KEY_OUT_3 = 1;
        KEY_OUT_4 = 0;
        break;
    default:
        break;
    }
}

void latticeListen()
{
    //注意这里的命名 多个静态的变量命名可能会导致变量名称覆盖的问题
    static unsigned char i = 0;

    TH0 = 0xFC;
    TL0 = 0x67;
    P0 = 0xFF;

    switch (i)
    {
    case 0:
        ADDR2 = 0;
        ADDR1 = 0;
        ADDR0 = 0;
        P0 = ledChar[index][0];
        i++;
        break;
    case 1:
        ADDR2 = 0;
        ADDR1 = 0;
        ADDR0 = 1;
        P0 = ledChar[index][1];
        i++;
        break;
    case 2:
        ADDR2 = 0;
        ADDR1 = 1;
        ADDR0 = 0;
        P0 = ledChar[index][2];
        i++;
        break;
    case 3:
        ADDR2 = 0;
        ADDR1 = 1;
        ADDR0 = 1;
        P0 = ledChar[index][3];
        i++;
        break;
    case 4:
        ADDR2 = 1;
        ADDR1 = 0;
        ADDR0 = 0;
        P0 = ledChar[index][4];
        i++;
        break;
    case 5:
        ADDR2 = 1;
        ADDR1 = 0;
        ADDR0 = 1;
        P0 = ledChar[index][5];
        i++;
        break;
    case 6:
        ADDR2 = 1;
        ADDR1 = 1;
        ADDR0 = 0;
        P0 = ledChar[index][6];
        i++;
        break;
    case 7:
        ADDR2 = 1;
        ADDR1 = 1;
        ADDR0 = 1;
        P0 = ledChar[index][7];
        i = 0;
        break;

    default:
        break;
    }
}
void move(direction)
{
    //移动点阵二维数组的for循环索引
    unsigned char j;
    unsigned char i;

    unsigned char frist;

    // 0  为上   1为下  左为2  右为3
    if (direction == 0)
    {
        for (j = 0; j < 8; j++)
        {
            frist = ledChar[j][0];
            for (i = 0; i < 8; i++)
            {
                ledChar[j][i] = ledChar[j][i + 1];
            }
            ledChar[j][7] = frist;
        }
        frame_column_index--;

        if (frame_column_index <= 1)
        {
            frame_column_index = 8;
        }
    }
    else if (direction == 1)
    {
        for (j = 0; j < 8; j++)
        {
            frist = ledChar[j][7];
            //说出来你可能不信 我这里i写成i<0了 为了找这个bug我花了半个小时
            for (i = 7; i > 0; i--)
            {
                ledChar[j][i] = ledChar[j][i - 1];
            }
            ledChar[j][0] = frist;
        }
        frame_column_index++;
        if (frame_column_index >= 8)
        {
            frame_column_index = 0;
        }
    }
    else if (direction == 2)
    {
        ledChar[index][frame_column_index] = 0xff;
        index++;
        if (index >= 8)
        {
            index = 0;
        }
        ledChar[index][frame_column_index] =column[index];
    }
    else if (direction == 3)
    {
        //根据行列取出亮点的坐标 然后把亮点变灭
        ledChar[index][frame_column_index] = 0xff;
        index--;
        if (index <= 0)
        {
            index = 7;
        }
        // //把新的坐标变亮
      ledChar[index][frame_column_index] =column[index];
    }
}