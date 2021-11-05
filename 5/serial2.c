#include <reg52.h>
#include <stdio.h>              //printf头文件
#define uc unsigned char 
#define uint unsigned int
 
uc flag,i,flag_t,s[50]="",j=0,flag_n=0;
uc code table[]="I get ";        
 
void init()
{
	TMOD=0x20;	  //定时器工作方式，选择了定时器1，工作方式2 八位初值自动重装的8位定时器。		 
	TH1=0xfd;	  //定时器1初值	,设置波特率为9600 晶振11.0529MHZ?
	TL1=0xfd;
	TR1=1;		  //开启定时器1
 
	SM0=0;
	SM1=1;		  //10位异步接收，（8位数据）波特率可变
    REN=1;		  //允许串行口接收位
	EA=1;	      //允许中断（总闸）
	ES=1;		  //允许串口中断
}
void main()
{
	init();
	while(1)
{
	  if(flag==1)
	{	  if(flag_n!=0)		   //使第二个及以后I get xx 换行，不与You transfer在一行（单纯为了格式好看）
			{TI=1;
			printf("\n");
			while(!TI);
			TI=0;
			}
		for(i=0;i<6;i++)
		{
			SBUF=table[i];
			while(!TI);
			TI=0;
		}
			for(i=0;s[i]!='#'&&i<50;i++)
		{
			SBUF=s[i];
			while(!TI);
			TI=0;
		}
		flag=0;
	}
	if(flag_t==1)						   //发送完毕之后，在电脑端输出。
	{
		TI=1;							   //printf之前必须将T1置为1才行。
		printf ("\nYou transfer %s",s);
		while(!TI);
		TI=0;
		flag_t=0;
	}	
}
}
 
void ser() interrupt 4
{
		if(RI)		 //接收数据，手动将RI清0
	{	    
		RI=0;
		
		if(flag==0&&j!=0)//1.循环赋值为'\0'(字符串结尾标志符),j=0，为了第二次传递字符串是又是从头输出
		{			 //2.flag为0和j不为0时，保证是第二次及以后，传输字符串（控制输出格式）
			flag_n++;			 
			for(j=0;s[j]!='#'&&j<50;j++)
				  s[j]='\0';
				  j=0;
		}
		s[j]=SBUF;
		flag=1;
		if(s[j]=='#'||j==49)	 //以'#'作为传送字符串的结尾符，我定义的字符数组最长为50所以49也应该结束。
			flag_t=1;
		else
		 	j++;
	}
 
	if(TI)     //发送数据
	{
	}	 
}