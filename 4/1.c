#include "reg52.h"
unsigned int i;
sbit ADDR0 = P1^0;
sbit ADDR1 = P1^1;
sbit ADDR2 = P1^2;
sbit ADDR3 = P1^3;
sbit ENLED = P1^4;
sbit LED8 = P0^6;
sbit LED7 = P0^5;
sbit LED6 = P0^4;
sbit LED5 = P0^3;
sbit LED4 = P0^2;
sbit LED3 = P0^1;
sbit LED2 = P0^0;
void main()
{
	P2=0xf0;
	ENLED = 0; //选择独立LED 进行显示
	ADDR3 = 1;
	ADDR2 = 1;
	ADDR1 = 1;
	ADDR0 = 0;
	while(1)
	{
		P2=0xf7;//2 led 8
		while(P2==0xd7)
		{
			for(i=10000;i>0;i--);
			if(P2==0xd7)
			{
				LED8=~LED8;
			}
		}
		P2=0xfb;//5 led5
		if(P2==0xeb)
		{
			for(i=2000;i>0;i--);
			if(P2==0xeb)
			{
				LED5=0;
			}
			else
			{
				LED5=1;
			}
		}
	}
}