#include <reg51.h>
#include <intrins.h>
#define uint unsigned int
#define uchar unsigned char
#define DATA P0 //LCD12864数据线       
sbit RS=P2^2; // 数据\指令 选择       
sbit RW=P2^1; // 读\写 选择          
sbit EN=P2^0; // 读\写使能       
sbit cs1=P2^4; // 片选1         
sbit cs2=P2^3; // 片选2         
/**********定义中文字库************/
uchar code Hzk[]={
0x10,0x10,0xD0,0xFF,0x90,0x10,0x00,0xFE,0x22,0x22,0x22,0x22,0x22,0xFF,0x02,0x00,
0x04,0x03,0x00,0xFF,0x00,0x01,0x00,0xFF,0x42,0x42,0x42,0x42,0x42,0xFF,0x00,0x00,
/*以上为"相"的点阵*/
0x80,0x40,0x20,0xF8,0x07,0x24,0x24,0x24,0x25,0x26,0x24,0x24,0xB4,0x26,0x04,0x00,
0x00,0x00,0x00,0xFF,0x00,0x01,0xFD,0x45,0x45,0x45,0x45,0x45,0xFD,0x01,0x00,0x00,
/*以上为"信"的点阵*/
0x00,0x00,0x00,0xF8,0x48,0x4C,0x4A,0x49,0x48,0x48,0x48,0xFC,0x08,0x00,0x00,0x00,
0x00,0x00,0x00,0xFF,0x44,0x44,0x44,0x44,0x44,0x44,0x44,0xFF,0x00,0x00,0x00,0x00,
/*以上为"自"的点阵*/
0x00,0x04,0x84,0x84,0x84,0x84,0x84,0x84,0x84,0x84,0x84,0xFE,0x04,0x00,0x00,0x00,
0x00,0x00,0x3F,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x41,0x40,0x70,0x00,0x00,
/*以上为"己"的点阵*/
0x40,0x48,0x48,0x48,0xFF,0x48,0x68,0x40,0x0C,0x30,0xC0,0x30,0x0E,0x00,0x00,0x00,
0x40,0x30,0x0F,0x08,0x1F,0x22,0x42,0x48,0x44,0x43,0x40,0x41,0x46,0x6C,0x20,0x00,
/*以上为"赵"的点阵*/
0x40,0x42,0xCC,0x00,0x04,0xFF,0x04,0x04,0xFC,0x00,0xFC,0x04,0x04,0xFE,0x04,0x00,
0x40,0x20,0x1F,0x28,0x44,0x43,0x48,0x50,0x4F,0x40,0x5F,0x48,0x48,0x6F,0x20,0x00,
/*以上为"迦"的点阵*/
0x04,0xE4,0x24,0x24,0x64,0xA4,0x24,0x3F,0x24,0xA4,0x64,0x24,0x24,0xF6,0x24,0x00,
0x00,0xFF,0x00,0x09,0x09,0x09,0x09,0x7F,0x09,0x09,0x09,0x49,0x80,0x7F,0x00,0x00,
/*以上为"南"的点阵*/
0x00,0x00,0x00,0x00,0x00,0x00,0x1C,0xFE,0xFE,0x1C,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x33,0x33,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
/*以上为"！"的点阵*/
};

/*状态检查，LCD是否忙*/
void CheckState()		
{
uchar dat;//状态信息（判断是否忙）
      RS=0; // 数据\指令选择，D/I（RS）="L" ，表示 DB7∽DB0 为显示指令数据 
      RW=1; //R/W="H"，E="H"数据被读到DB7∽DB0              
do{
     DATA=0x00;
     EN=1;	//EN下降源      
	_nop_(); //一个短延时     
	dat=DATA;
     EN=0;
dat=0x80 &dat; //仅当第7位为0时才可操作(判别busy信号)          
}while(!(dat==0x00));
}
 /*写命令到LCD中*/
SendCommandToLCD(uchar com)
{
CheckState();//状态检查，LCD是否忙       
   RS=0;	 //向LCD发送命令。RS=0写指令，RS=1写数据         
   RW=0;//R/W="L" ，E="H→L"数据被写到 IR 或 DR 
   DATA=com;	 //com :命令       
   EN=1;   
   _nop_();
   _nop_(); 
   EN=0;  //EN下降源  
   return 0;//后加入  
}
 /*设置页 0xb8是页的首地址*/
void SetLine(uchar page)	
{
  page=0xb8|page; //1011 1xxx 0<=page<=7 设定页地址--X 0-7,8行为一页64/8=8，共8页              
SendCommandToLCD(page);
}
 /*设定显示开始行，0xc0是行的首地址*/          
void SetStartLine(uchar startline) 	
{
startline=0xc0|startline; //1100 0000     
SendCommandToLCD(startline); //设置从哪行开始：0--63，一般从0 行开始显示           
}
 /*设定列地址--Y 0-63 ，0x40是列的首地址*/             
void SetColumn(uchar column)	
{
  column=column &0x3f; //column最大值为64，越出 0=<column<=63         
column= 0x40|column; //01xx xxxx
SendCommandToLCD(column);
}
 /*开关显示，0x3f是开显示，0x3e是关显示*/
void SetOnOff(uchar onoff)	
{
	onoff=0x3e|onoff; //0011 111x,onoff只能为0或者1        
	SendCommandToLCD(onoff);
}
/*写显示数据 */
void WriteByte(uchar dat)	
{
CheckState();//状态检查，LCD是否忙       
   RS=1; //RS=0写指令，RS=1写数据        
   RW=0;////R/W="L" ，E="H→L"数据被写到 IR 或 DR 
   DATA=dat;//dat:显示数据     
   EN=1; 
   _nop_();
   _nop_();
   EN=0;//EN下降源
}
/*选择屏幕screen: 0-全屏,1-左屏,2-右屏*/
void SelectScreen(uchar screen)	
{ 						
switch(screen)
   { case 0: cs1=0;//全屏显示  
             _nop_(); _nop_(); _nop_(); 
             cs2=0; 
             _nop_(); _nop_(); _nop_(); 
break; 
     case 1: cs1=0;//左显示屏    
             _nop_(); _nop_(); _nop_(); 
             cs2=1;
             _nop_(); _nop_(); _nop_(); 
break;
     case 2: cs1=1;//右显示屏     
             _nop_(); _nop_(); _nop_(); 
             cs2=0;
             _nop_(); _nop_(); _nop_(); 
break;
   }
}
 /*清屏screen: 0-全屏,1-左屏,2-右*/                    
void ClearScreen(uchar screen)	
{ 	
uchar i,j;
SelectScreen(screen);
	 for(i=0;i<16;i++)	 //控制页数0-7，共8页      
	 {
	SetLine(i);
			SetColumn(0);
              for(j=0;j<64;j++)	   //控制列数0-63，共64列       
			  {
WriteByte(0x00); //写点内容，列地址自动加1       
              }
     } 
}
/*延时程序*/
void	delay(uint z)		
{
	uint i,j;
	for(i=0; i<z; i++)
		for(j = 0; j < 110; j++);
}
/*初始化LCD*/
void InitLCD() 	
{ 
	CheckState();
	SelectScreen(0);
	SetOnOff(0); //关显示     
	SelectScreen(0);
	SetOnOff(1); //开显示   
	SelectScreen(0);         
	ClearScreen(0);//清屏   
	SetStartLine(0); //开始行:0    
}
/*显示全角汉字*/ 
 void Display(uchar ss,uchar page,uchar column,uchar number) 
 {
	 int i; //选屏参数，page选页参数，column选列参数，number选第几汉 字输出 
	 SelectScreen(ss);
	 column=column&0x3f; 
	 SetLine(page); //写上半页 
   SetColumn(column); //控制列 
 for(i=0;i<16;i++) //控制16列的数据输出
 {
	WriteByte(Hzk[i+32*number]);
 } //i+32*number汉字的前16个数据输出 
	SetLine(page+1); //写下半页 
	SetColumn(column); //控制列 
	for(i=0;i<16;i++) //控制16列的数据输出
       {
          WriteByte(Hzk[i+32*number+16]);}//i+32*number+16汉字的后16个数据 输出
       }

/*主函数*/
void main()
{  
	uint i;
	InitLCD();//初始12864      
	ClearScreen(0);//清屏    
	while(1)
	{   
	      for(i=0;i<128;i++) //12864有128列，显示开始行从0~128切换，因此可以实现循环显示
            {
SetStartLine(i);//显示开始行     
		Display(2,0,0*16,0);//显示第一个字
		Display(2,0,1*16,1);//显示第二个字
		Display(2,0,2*16,2);//显示第三个字
  	    Display(2,0,3*16,3);//显示第四个字
		Display(1,0,4*16,4);//显示第五个字
		Display(1,0,5*16,5);//显示第六个字
		Display(1,0,6*16,6);//显示第七个字
		Display(1,0,7*16,7);//显示第八个字
SelectScreen(0);//选择全屏显示
	      delay(50); //延时 
      }
}
}