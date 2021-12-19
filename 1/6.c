#include <reg52.h>
/* 全局运行参数定义 */
#define SYS_MCLK   (11059200/12)  //系统主时钟频率，即振荡器频率÷12

/*蜂鸣器相关*/
bit staBuzzer = 0; //蜂鸣器状态控制位，1-鸣叫、0-关闭
sbit BUZZER = P1^6;  //蜂鸣器控制引脚
void Waring();

/*--------------------------------LCD1602函数声明---------------------------*/
//LCD1602函数声明
void InitLcd1602();									//初始化
void LcdWriteCmd(unsigned char cmd);					//写命令
void LcdSetCursor(unsigned char x, unsigned char y);	//写指针
void LcdShowStr(unsigned char x, unsigned char y,unsigned char *str, unsigned char len);

/*--------------------------------EEPROM读写函数----------------------------*/
void E2Read(unsigned char *buf, unsigned char addr, unsigned char len);
void E2Write(unsigned char *buf, unsigned char addr, unsigned char len);

/*--------------------------------按键驱动函数声明---------------------------*/
void KeyDriver();		//按键变化检测

/*--------------------------------密码输入函数声明---------------------------*/
void inputpassword();	//密码输入特效支持（智能隐藏）

/*--------------------------------系统控制函数声明---------------------------*/
void control();

#define PWN 16										/*密码位数*/
unsigned char code PassWordAddr = 0x17;				//密码储存地址
/*密码存储格式：前两个固定字符：PW，P为0x17，PW后紧接密码（字符）*/
unsigned char InputStr[PWN] =							//输入的密码
{' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '};
unsigned char PassWord[PWN];							//正确的密码
unsigned char system = 0;								//流程控制变量
/*0:开始，1设置密码，2输入密码，3显示密文*/
bit PWRight  = 0;		//密码是否对
bit EnterSta = 0;		//回车状态

void main()
{
	TMOD = 0x01;
	TH0 = 0xFC;
	TL0 = 0x66;
	EA = 1;
	ET0 = 1;
	TR0 = 1;

	
    T2CON = 0x00;  //配置T2工作在16位自动重载定时器模式
	RCAP2H = ((65536-SYS_MCLK/1500)>>8);  //配置重载值，每秒产生1500次中断，
    RCAP2L = (65536-SYS_MCLK/1500);       //以使刷新率达到100Hz无闪烁的效果
    TH2 = RCAP2H;  //设置初值等于重载值
    TL2 = RCAP2L;
    ET2 = 1;       //使能T2中断
    PT2 = 1;       //设置T2中断为高优先级
    TR2 = 1;       //启动T2
	
	InitLcd1602();
	
	//E2Write(PWhead,PassWordAddr,2);		//去掉密码头
	
	while(1)
	{
		control();
	}
}
/*--------------------------------系统控制函数声明---------------------------*/
void control()
{
	static unsigned char systemb = 0;
	if(system == 0)						//系统刚启动
	{
		unsigned char PWhead[2] = {0,0};
		if(system != systemb)
		{
			systemb = system;
			LcdWriteCmd(0x01);	//清屏
			system = systemb;
			LcdShowStr(0,0,"Loading......   ", 16);
		}
		
		E2Read(PWhead,PassWordAddr,2);	//读密码头
		if((PWhead[0] == 'P')&&(PWhead[1] == 'W'))	//存在密码头
		{
			E2Read(PassWord,PassWordAddr+2,sizeof(PassWord));//读密码
			system = 2;				//输入密码
		}else{										//不存在密码头
			system = 1;				//设置密码
		}
	}
	
	if(system == 1)	//如果是设置密码
	{
		if(system != systemb)
		{
			systemb = system;
			LcdWriteCmd(0x01);	//清屏
			system = systemb;
			LcdShowStr(0,1,"Set Password!   ", 16);
		}
		
		inputpassword();	//运行密码特效
		KeyDriver();		//运行按键驱动
		if((PWRight == 1)&&(EnterSta == 1))
		{
			PWRight = 0;
			EnterSta = 0;
			if((InputStr[0] == ' ')||((InputStr[0]<('9'+1))&&(InputStr[0]>('0'-1))))
			//如果是合法的密码
			{
				//写密码头
				E2Write("PW",PassWordAddr,2);
				
				//跳过密码头写密码
				E2Write(InputStr,(PassWordAddr+2),sizeof(InputStr));
				
				system = 0;	//重新开始系统
			}
		}
	}
	
	if(system == 2)	//如果是输入密码
	{
		if(system != systemb)
		{
			systemb = system;
			LcdWriteCmd(0x01);	//清屏
			system = systemb;
			LcdShowStr(0,1,"Enter Password! ", 16);
			//LcdShowStr(0,0,PassWord, PWN);//直接显示密码
		}
		inputpassword();	//运行密码特效
		KeyDriver();		//运行按键驱动			
		if((PWRight == 1)&&(EnterSta == 1))	//如果密码正确
		{
			PWRight = 0;
			EnterSta = 0;
			system = 3;			//显示密文
		}
	}
	
	if(system == 3)		//显示密文
	{
		if(system != systemb)
		{
			systemb = system;
			LcdShowStr(0,0,"Pwd is correct! ", 16);
		}
		KeyDriver();		//运行按键驱动
	}
	
	Waring();//报警
}

/*-------------------------密码输入控制部分----------------------------------*/
unsigned char inputI = 0;								//输入指针
bit clear = 1;				//变*标志位，0为到1s
bit time1 = 0;				//密码延时定时器
unsigned int cnt = 0;			//延时计数器
unsigned char code staR = '*';
unsigned char code blank = ' ';
unsigned char AinputI;		//打印inputI 

void esc()		//按下清除键
{
	switch(system)
	{
		case 1:			//输入密码的时候
		case 2:	
			if(inputI != 0)
			{
				time1 = 0;	//停止变*
				inputI--;
				LcdShowStr(inputI,0,&blank,1);
				InputStr[inputI] = blank;
			}
			break;
		
		case 3:
			system = 2;
			break;
		
		default: break;
	}
}

//可以和按键一起放到1ms定时器中调用
void pwtime()
{
	if(time1 == 1)		//如果在运行
	{
		cnt++;
		if(cnt>1000)	//1s到了
		{
			cnt = 0;
			clear = 0;	//超时变成*
			time1 = 0;	//关闭密码延时定时器	
		}
	}
}

void inputpassword()
{
	static unsigned char inputIb = 1;		//输入指针备份
	
	if(clear == 0)
	{
		clear = 1;	//取消标志	
		if(inputI > 0 )
				LcdShowStr(inputI-1,0,&staR,1);
	}
	
	if(inputIb != inputI)	//如果有新输入
	{
		inputIb = inputI;	//备份
		
		AinputI = (inputI + '0');
		
		if(inputI>1)		//上一个字符马上变成*
		{
			LcdShowStr(inputI-2,0,&staR,1);
		}
				
		time1 = 1;			//打开延时定时器
		cnt = 0;				//清零计数
		
		if(inputIb == 0)		//清空了输入
		{
			time1 = 0;	//停止变*
		}
		
		LcdShowStr(15,1,&AinputI,1);	//打印inputI 
		
		LcdSetCursor(inputI, 0);			//光标控制
		//LcdWriteCmd(0x0E);			//光标模式，这里在初始化里写了					
	}
}

//字符串清空函数，*p字符串，n清空的个数
void empty(unsigned char *p,unsigned char n)	
{
	unsigned char i;
	for(i = 0;i<n;i++)
	{
		(*p++) = blank;		//依次清空
	}
}

//密码核对函数，直接对比InputStr[i]，PassWord[i]的内容
void isright()		
{
	unsigned char i;		//循环对比
	
	EnterSta = 1;
	
	switch(system)
	{
		case 1:
			PWRight = 1;		
			inputI = 0;									//输入指针归零
		break;
		
		case 2:
			for(i = 0;i<PWN;i++)
			{
				if(InputStr[i] != PassWord[i])	//如有不同
					break;				//退出
			}
			
			if(i == PWN)									//如果有PWN个相同
			{
				LcdShowStr(0,1,"True!           ",16);		//提示密码正确
				PWRight = 1;
			}else{
				LcdShowStr(0,1,"False!          ",16);		//提示密码错误
				PWRight = 0;
				staBuzzer = 1;//报警!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			}
			inputI = 0;									//输入指针归零
			
			empty(InputStr,sizeof(InputStr));			//清空输入
			LcdShowStr(0,0,InputStr,sizeof(InputStr));	//显示清空后的输入
		break;
			
		case 3:
			system = 1;
		break;
		
		default:break;
	}
	
//	
//	if(system == 3)		//查看密文时，按enter键修改密码
//	{
//		system = 1;
//	}
//	
//	if(system == 2)		//输入密码状态
//	{
//		for(i = 0;i<PWN;i++)
//		{
//			if(InputStr[i] != PassWord[i])	//如有不同
//				break;				//退出
//		}
//		
//		if(i == PWN)									//如果有PWN个相同
//		{
//			LcdShowStr(0,1,"Right!          ",16);		//提示密码正确
//			PWRight = 1;
//		}else{
//			LcdShowStr(0,1,"Try again!      ",16);		//提示密码错误
//			PWRight = 0;
//		}
//		inputI = 0;									//输入指针归零
//		
//		empty(InputStr,sizeof(InputStr));			//清空输入
//		LcdShowStr(0,0,InputStr,sizeof(InputStr));	//显示清空后的输入
//	}
//	
//	if(system == 1)		//如果是设置密码
//	{
//		PWRight = 1;		
//		inputI = 0;									//输入指针归零
//	}
}
#undef PWN				//结束宏定义

//密码输入控制函数
void InputSetNumber(unsigned char ccode)
{
	if(inputI<16)
	{
		if  ((ccode>='0') && (ccode<='9'))  	//数字键输入当前位设定值
		{	
			InputStr[inputI] = ccode;		//保存
			LcdShowStr(inputI,0,&ccode,1);	//显示输入
			
			inputI++;						//输入指针+1
		}
	}
}
void SetLeftShift()
{
	
}
void SetRightShift()
{
	
}
/*--------------------------以下是蜂鸣器报警部分-------------------------------*/
bit buti = 0;//蜂鸣器定时器状态
bit buSta = 0;//蜂鸣器状态
unsigned char bucnout = 0;//蜂鸣器计数器
unsigned char butimes = 0;//响声计次
void Waring()
{
	if(staBuzzer == 1)
	{
		butimes = 6;//警报响的次数
		staBuzzer = 0;
		buti = 1;//开始计数		
	}	
	if(butimes)
	{
		if(bucnout>127)
		{
			buSta = 0;
		}else{
			buSta = 1;
		}
	}
}

void buzzTime()
{
	if(buti == 1)
	{
		bucnout++;
		if(bucnout>254)
		{
			bucnout = 0;

			if(butimes == 0)
			{
				buti = 0;	
			}else{
				butimes--;
			}
		}	
	}
}

/*--------------------------以下是矩阵按键驱动-------------------------------*/
sbit KEY_IN_1  = P2^4;
sbit KEY_IN_2  = P2^5;
sbit KEY_IN_3  = P2^6;
sbit KEY_IN_4  = P2^7;
sbit KEY_OUT_1 = P2^3;
sbit KEY_OUT_2 = P2^2;
sbit KEY_OUT_3 = P2^1;
sbit KEY_OUT_4 = P2^0;

unsigned char code KeyCodeMap[4][4] = {  //矩阵按键到标准键码的映射表
    { '1',  '2',  '3', 0x26 },  //数字键1、数字键2、数字键3、向上键
    { '4',  '5',  '6', 0x25 },  //数字键4、数字键5、数字键6、向左键
    { '7',  '8',  '9', 0x28 },  //数字键7、数字键8、数字键9、向下键
    { '0', 0x1B, 0x0D, 0x27 }   //数字键0、ESC键、  回车键、 向右键
};
unsigned char KeySta[4][4] = {  //全部矩阵按键的当前状态
    {1, 1, 1, 1},  {1, 1, 1, 1},  {1, 1, 1, 1},  {1, 1, 1, 1}
};

void KeyAction(unsigned char keycode)
{
	if  ((keycode>='0') && (keycode<='9'))  //数字键输入当前位设定值
	{
		InputSetNumber(keycode);//调用字符串储存函数，用于存输入的密码
	}
	else if (keycode == 0x25)  //向左键，向左切换设置位
	{
		SetLeftShift();
	}
	else if (keycode == 0x27)  //向右键，向右切换设置位
	{
		SetRightShift();
	}
	else if (keycode == 0x0D)  //回车键，切换运行状态/保存设置
	{
		isright();		//调用密码核对
	}
	else if (keycode == 0x1B)  //Esc键，静音/取消当前设置
	{
		esc();
	}
}

//按键驱动函数，检测按键动作，调度相应动作函数，需在主循环中调用 
void KeyDriver()
{
	unsigned char i, j;
	static unsigned char backup[4][4] = {  //按键值备份，保存前一次的值
		{1, 1, 1, 1},  {1, 1, 1, 1},  {1, 1, 1, 1},  {1, 1, 1, 1}
	};

	for (i=0; i<4; i++)  //循环检测4*4的矩阵按键
	{
		for (j=0; j<4; j++)
		{
			if (backup[i][j] != KeySta[i][j])    //检测按键动作
			{
				if (backup[i][j] != 0)           //按键按下时执行动作
				{
					KeyAction(KeyCodeMap[i][j]); //调用按键动作函数
				}
				backup[i][j] = KeySta[i][j];     //刷新前一次的备份值
			}
		}
	}
}

// 按键扫描函数，需在定时中断中调用，推荐调用间隔1ms 
void KeyScan()
{
	unsigned char i;
	static unsigned char keyout = 0;   //矩阵按键扫描输出索引
	static unsigned char keybuf[4][4] = {  //矩阵按键扫描缓冲区
		{0xFF, 0xFF, 0xFF, 0xFF},  {0xFF, 0xFF, 0xFF, 0xFF},
		{0xFF, 0xFF, 0xFF, 0xFF},  {0xFF, 0xFF, 0xFF, 0xFF}
	};

    //将一行的4个按键值移入缓冲区
	keybuf[keyout][0] = (keybuf[keyout][0] << 1) | KEY_IN_1;
	keybuf[keyout][1] = (keybuf[keyout][1] << 1) | KEY_IN_2;
	keybuf[keyout][2] = (keybuf[keyout][2] << 1) | KEY_IN_3;
	keybuf[keyout][3] = (keybuf[keyout][3] << 1) | KEY_IN_4;
    //消抖后更新按键状态
	for (i=0; i<4; i++)  //每行4个按键，所以循环4次
	{
		if ((keybuf[keyout][i] & 0x0F) == 0x00)
		{   //连续4次扫描值为0，即4*4ms内都是按下状态时，可认为按键已稳定的按下
			KeySta[keyout][i] = 0;
		}
		else if ((keybuf[keyout][i] & 0x0F) == 0x0F)
		{   //连续4次扫描值为1，即4*4ms内都是弹起状态时，可认为按键已稳定的弹起
			KeySta[keyout][i] = 1;
		}
	}
	//执行下一次的扫描输出
	keyout++;        //输出索引递增
	keyout &= 0x03;  //索引值加到4即归零
	switch (keyout)  //根据索引值，释放当前输出引脚，拉低下次的输出引脚
	{
		case 0: KEY_OUT_4 = 1; KEY_OUT_1 = 0; break;
		case 1: KEY_OUT_1 = 1; KEY_OUT_2 = 0; break;
		case 2: KEY_OUT_2 = 1; KEY_OUT_3 = 0; break;
		case 3: KEY_OUT_3 = 1; KEY_OUT_4 = 0; break;
		default: break;
	}
}

/*-----------------------以下是LCD1602显示驱动-------------------------------*/
//定义LCD1602显示相关的IO接口
#define LCD1602_DB  P0
sbit LCD1602_RS = P1^0;
sbit LCD1602_RW = P1^1;
sbit LCD1602_E  = P1^5;

//等待液晶准备好
void LcdWaitReady()
{
    unsigned char sta;
     
    LCD1602_DB = 0xFF;
    LCD1602_RS = 0;
    LCD1602_RW = 1;
    do {
        LCD1602_E = 1;      //使能LCD1602
        sta = LCD1602_DB;   //读取状态字
        LCD1602_E = 0;      //避免干扰其他器件
    } while (sta & 0x80);   //第七位等于1表示液晶正忙，重复检测直到第七位等于0为止
}
//向LCD1602液晶写入一字节命令，cmd为待写入命令值
void LcdWriteCmd(unsigned char cmd)
{
    LcdWaitReady();
    LCD1602_RS = 0;
    LCD1602_RW = 0;
    LCD1602_DB = cmd;
    LCD1602_E  = 1;
    LCD1602_E  = 0;
}
//向LCD1602液晶写入一字节数据，dat为待写入数据值
void LcdWriteDat(unsigned char dat)
{
    LcdWaitReady();
    LCD1602_RS = 1;
    LCD1602_RW = 0;
    LCD1602_DB = dat;
    LCD1602_E  = 1;
    LCD1602_E  = 0;
}
//设置显示RAM起始地址，亦即光标位置，(x,y)对应屏幕上的字符的坐标
void LcdSetCursor(unsigned char x, unsigned char y)
{
    unsigned char addr;
     
    if (y == 0)  			//由输入的屏幕坐标计算显示RAM的地址
        addr = 0x00 + x;	//第一行字符地址从0x00起始
    else
        addr = 0x40 + x;	//第二行字符地址从0x40起始
    LcdWriteCmd(addr | 0x80);	//设置RAM地址
}
//在液晶上显示字符串，(x,y)-对应屏幕上的起始坐标，
//str-字符串指针，len-需显示的字符长度
void LcdShowStr(unsigned char x, unsigned char y,unsigned char *str, unsigned char len)
{
	LcdSetCursor(x, y);   //设置起始地址
	while (len--)         //连续写入len个字符数据
	{
		LcdWriteDat(*str++);  //先取str指向的数据，然后str自加1
	}
}

void InitLcd1602()//初始化1602液晶
{
    LcdWriteCmd(0x38);  //16*2显示，5*7点阵，8位数据接口
    LcdWriteCmd(0x0E);  //显示器开，光标打开
    LcdWriteCmd(0x06);  //文字不动，地址自动+1
    LcdWriteCmd(0x01);  //清屏
}

/*--------------------------------以下是I2C总线协议--------------------------*/
#include <intrins.h>
sbit I2C_SDA = P3^6;
sbit I2C_SCL = P3^7;

//I2C延时函数
void I2CDelay()
{
	_nop_();	//1us
	_nop_();
	_nop_();
	_nop_();
}

//I2C起始信号
void I2CStart()
{
	I2C_SDA = 1;	//确保SDA，SCL为高电平
	I2C_SCL = 1;
	I2CDelay();	//延时，让其他器件识别
	I2C_SDA = 0;	//先拉低SDA
	I2CDelay();
	I2C_SCL = 0;	//再拉低SCL
}

//I2C写函数，dat：要写入的字节，返回为应答值：1应答、0非应答
bit I2CWrite(unsigned char dat)
{
	unsigned char mask;		//临时变量
	bit ack;					//应答值变量
	for(mask = 0x80;mask != 0;mask>>=1)	//从高位到低位依次取值
	{
		if((mask&dat) == 0)		//如果是0
			I2C_SDA = 0;	//写0
		else
			I2C_SDA = 1;	//写1
		I2CDelay();		//等其他器件反应
		I2C_SCL = 1;		//让其他器件读取
		I2CDelay();		//其他器件读取要时间，等
		I2C_SCL = 0;		//准备发送下一位
	}
	I2C_SDA = 1;		//发送完后释放总线，以检测从机应答
	I2CDelay();		//等其他器件反应
	I2C_SCL = 1;		//让其他器件发送应答位
	ack = I2C_SDA;	//读取应答位
	I2CDelay();		//等待
	I2C_SCL = 0;		//保持住总线，以进行其他操作
	
	return (~ack);	//返回应答位，1成功，0失败
}

//I2C停止信号
void I2CStop()
{
	I2C_SDA = 0;	//确保SDA，SCL为低电平
	I2C_SCL = 0;
	I2CDelay();	//等待其他器件反应
	I2C_SCL = 1;	//先拉高SCL
	I2CDelay();
	I2C_SDA = 1;	//再拉高SDA
	I2CDelay();
}

//I2C读并发送非应答信号函数，返回读到的字节
unsigned char I2CReadNAK()
{
	unsigned char dat,mask;	//临时变量，dat存数据，mask循环辅助
	
	I2C_SDA = 1;				//释放总线，读操作，读IO口置1
	for(mask = 0x80;mask != 0;mask >>= 1)	//从高位依次读取
	{
		I2CDelay();			//等待SDA被其他器件送入要读取的数据
		I2C_SCL = 1;			//表示要读了
		if(I2C_SDA == 0)		//读数据
			dat &= ~mask;	//读到0，当前位置零
		else
			dat |= mask;		//读到1，当前位置壹
		I2CDelay();			//等其他器件反应
		I2C_SCL = 0;			//让从机准备发送下一位
	}
	I2C_SDA = 1;		//准备发送非应答信号
	I2CDelay();
	I2C_SCL = 1;		//让从机接收应答位
	I2CDelay();
	I2C_SCL = 0;		//保持住总线，准备下一个操作
	
	return dat;			//返回收到的数据
}

//I2C读并发送应答信号函数，返回读到的字节
unsigned char I2CReadACK()
{
	unsigned char dat,mask;	//临时变量，dat存数据，mask循环辅助
	
	I2C_SDA = 1;			//释放总线，读操作，读IO口置1
	for(mask = 0x80;mask != 0;mask >>= 1)	//从高位依次读取
	{
		I2CDelay();		//等待SDA被其他器件送入要读取的数据
		I2C_SCL = 1;	//表示要读了
		if(I2C_SDA == 0)	//读数据
			dat &= ~mask;	//读到0，当前位置零
		else
			dat |= mask;	//读到1，当前位置壹
		I2CDelay();		//等其他器件反应
		I2C_SCL = 0;	//让从机准备发送下一位
	}
	I2C_SDA = 0;		//准备发送应答信号（与NAK就这不同）
	I2CDelay();
	I2C_SCL = 1;		//让从机接收应答位
	I2CDelay();
	I2C_SCL = 0;		//保持住总线，准备下一个操作
	
	return dat;		//返回收到的数据
}

/*---------------------------以下是EEPROM驱动-------------------------------*/
//EEPROM读取函数，buf：读取出的数据储存指针，
//addr：EEPROM中读数据的起始地址，len：读取数据的长度
void E2Read(unsigned char *buf, unsigned char addr, unsigned char len)
{
	do {						//用寻址检查当前是否可进行读操作
		I2CStart();
		if (I2CWrite(0x50<<1))	//如果应答（表示不忙）
		{
			break;				//跳出循环
		}
		I2CStop();
	} while(1);
	I2CWrite(addr);				//写入起始地址

	I2CStart();					//重复发送启动信号
	I2CWrite((0x50<<1)|0x01);	//寻址，读操作
	while (len > 1)				//连续读取len-1个字节
	{
		*buf++ = I2CReadACK();	//还没到最后一个字节，发送应答信号
		len--;
	}
	*buf = I2CReadNAK();			//最后一个字节，发送非应答
	I2CStop();					//发送停止信号
}

//EEPROM写入数据函数，buf：源数据指针，addr：EEPROM中的起始地址，len：写入长度
void E2Write(unsigned char *buf, unsigned char addr, unsigned char len)
{
	while (len > 0)
	{
		//等待上次写入操作完成
		do {							//用寻址操作查询当前是否可进行读写操作
			I2CStart();
			if (I2CWrite(0x50<<1))	//应答则跳出循环，非应答则进行下一次查询
			{
				break;
			}
			I2CStop();
		} while(1);

		//按页写模式连续写入字节
		I2CWrite(addr);				//写入起始地址
		while (len > 0)
		{
			I2CWrite(*buf++);		//写入一个字节数据
			len--;					//待写入长度计数递减
			addr++;					//EEPROM存储地址递增
			if ((addr&0x07) == 0)		//检查地址是否到达页边界，24C02每页8字节，检测低3位是否为零即可
			{
				break;				//到达页边界时，跳出循环，结束本次写操作
			}
		}
		I2CStop();
	}
}

/*------------------------------中断函数------------------------------------*/
/* T0中断服务函数，按键扫描、计时器、蜂鸣器控制 */
void InterruptTimer0() interrupt 1
{
	TH0 = 0xfc;
	TL0 = 0x66;
	KeyScan();
	pwtime();
	buzzTime();
}

/* T2中断服务函数，LED动态扫描、蜂鸣器控制 */
void InterruptTimer2() interrupt 5
{
    TF2 = 0;  //清零T2中断标志
    
    //由蜂鸣器状态位控制蜂鸣器
    if (buSta == 1)
        BUZZER = ~BUZZER;  //蜂鸣器鸣叫
    else
        BUZZER = 1;        //蜂鸣器静音
}