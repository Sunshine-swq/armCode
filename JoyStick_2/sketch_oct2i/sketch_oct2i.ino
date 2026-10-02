
#include<Servo.h>


int Ljoyx=A3;  //左侧摇杆x轴信号
int Ljoyy=A2;  //左侧y轴

int Rjoyx=A1;  //右侧x轴
int Rjoyy=A0;  //右侧y轴

Servo servos[4];// 0——>claw  1——>rarm  2——>farm  3——>base
int attach_Pin[4]={6,9,10,11};

int currentPos[4]={90,90,90,90}; //电机当前位置
int targetPos[4]={90,90,90,90};  //电机期望位置


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  for(int i=0;i<4;i++)
  {
      servos[i].attach(attach_Pin[i]);   //设定引脚
      servos[i].write(90);               //初始化
  }
  delay(500);
}

void loop() {
  // put your main code here, to run repeatedly:
   int toPos[4];
   toPos[0]=judgeStill(analogRead(Rjoyy),512,30);//获取摇杆信号  ——>claw
   toPos[1]=judgeStill(analogRead(Ljoyy),512,30);// rarm
   toPos[2]=judgeStill(analogRead(Rjoyx),512,30);// farm
   toPos[3]=judgeStill(analogRead(Ljoyx),512,30);// base

   for(int i=0;i<4;i++)    //将目标位置转化为角度
       targetPos[i]=map(toPos[i],0,1023,0,180);
   
   for(int i=0;i<4;i++)
   {
      if(currentPos[i]<targetPos[i]){ //大了就加
          currentPos[i]++;
      }
      else if(currentPos[i]>targetPos[i]) //小了就减
      {
          currentPos[i]--;
      }

      servos[i].write(currentPos[i]);
   }
  
   delay(5);



}


int judgeStill(int now,int center,int range)  //防中央抖动
{
    if(now>center-range && now<center+range)
    {
       return center;
    }
    return now;
}