#include	"config.h"
#include	"STC8G_H_GPIO.h"
#include	"STC8G_H_Delay.h"
#include    "STC8G_H_EEPROM.h"

//掉电存储 0关闭 1开启
#define POWER_DOWN_STORAGE 1
//按钮持续时间，即按下多少ms再松开才判断按钮按下
#define BUTTON_TIME 100

u8 mode[2]={0,0};
int timecnt=0;
void GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.Pin  = GPIO_Pin_3;
	GPIO_InitStructure.Mode = GPIO_OUT_PP;
	GPIO_Inilize(GPIO_P3,&GPIO_InitStructure);
	GPIO_InitStructure.Pin  = GPIO_Pin_4|GPIO_Pin_5;
	GPIO_InitStructure.Mode = GPIO_OUT_PP;
	GPIO_Inilize(GPIO_P5,&GPIO_InitStructure);
	
	GPIO_InitStructure.Pin  = GPIO_Pin_2;
	GPIO_InitStructure.Mode = GPIO_PullUp;
	GPIO_Inilize(GPIO_P3,&GPIO_InitStructure);
	//默认请求电压5V
	P54=1;//CFG1
	P55=0;//CFG2
	P33=0;//CFG3
}
//请求电压模式
//0-5V 1-9V 2-12V 3-15V 4-20V
void RequestVoltageMode(u8 mode)
{
	if(mode<0||mode>4)mode=0;
	switch(mode){
		case 0:
			P54=1;//CFG1
			P55=0;//CFG2
			P33=0;//CFG3
			break;
		case 1:
			P54=0;//CFG1
			P55=0;//CFG2
			P33=0;//CFG3
			break;
		case 2:
			P54=0;//CFG1
			P55=0;//CFG2
			P33=1;//CFG3
			break;
		case 3:
			P54=0;//CFG1
			P55=1;//CFG2
			P33=1;//CFG3
			break;
		case 4:
			P54=0;//CFG1
			P55=1;//CFG2
			P33=0;//CFG3
			break;
	}
}
void main(void)
{
	GPIO_Init();
#if POWER_DOWN_STORAGE
	EEPROM_read_n(0,mode,1);
	if(mode[0]<0||mode[0]>4){
		mode[0]=0;
		mode[1]=mode[0];
		RequestVoltageMode(mode[0]);
		EEPROM_SectorErase(0);
		EEPROM_write_n(0,mode,1);
	}else{
		RequestVoltageMode(mode[0]);
		mode[1]=mode[0];
	}
#else
	mode[0]=0;
	RequestVoltageMode(mode[0]);
	mode[1]=mode[0];
#endif
	
	while(1)
	{
		while(!P32){
			timecnt++;
			delay_ms(1);
		}
		if(timecnt>BUTTON_TIME){
			if(mode[0]<4)mode[0]++;
			else mode[0]=0;
		}
		timecnt=0;
		if(mode[1]!=mode[0]){
			if(mode[0]<0||mode[0]>4)mode[0]=0;
			RequestVoltageMode(mode[0]);
			mode[1]=mode[0];
#if POWER_DOWN_STORAGE
			EEPROM_SectorErase(0);
			EEPROM_write_n(0,mode,1);
#endif
		}
	}
}
