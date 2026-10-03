
#include<Servo.h>

int DSD=15;

int baseMin,baseMax;
int rarmMin,rarmMax;
int farmMin,farmMax;
int clawMin,clawMax;


//录制-------------------------------------------------------
#define MAXRECORD 300  //最大录制存储数
byte recordAngle[MAXRECORD][4];  //存储4个舵机的角度
int sampleCnt=0;  //录制的张数
bool isRecord=0;  //是处于录制还是处于放映
const int record_tab=75;  //每次存储的间隔75ms
int lastTime=0;
//————————————————————————————————————————————————————————————


Servo farm,rarm,claw,base; //设置舵机

int know_loop=0; //知道按键按下时执行移动哪个物体

char getCmd;    //获取指令

//摇杆区----------------------------------------------------------

int Ljoyx=A3;  //左侧摇杆x轴信号
int Ljoyy=A2;  //左侧y轴
int Rjoyx=A1;  //右侧x轴
int Rjoyy=A0;  //右侧y轴

int now_farmPos=90,now_rarmPos=90;  //舵机的初始位置
int now_clawPos=90,now_basePos=90;

int end_farmPos=90,end_rarmPos=90;  //舵机的终点位置
int end_clawPos=90,end_basePos=90;

//————————————————————————————————————————————————————————————————



void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  farm.attach(10);
  delay(15);
  rarm.attach(9);
  delay(15);
  base.attach(11);
  delay(15);
  claw.attach(6);
  delay(15);

  claw.write(90);
  farm.write(90);
  rarm.write(90);
  base.write(90);

  delay(500);

}

void loop() {
  // put your main code here, to run repeatedly:
   joyStickPlay();
   
   
   if(Serial.available()>0){
       getCmd=Serial.read();
       switch(getCmd){
           case 'A':
               loop_task2(know_loop%3);  //按键一  循环任务2
               know_loop++;
               break;
           case 'B':
               record();      //按键二  录制
               break;
           case 'C':
               release();     //按键三  回放
               break;
           case 'D':
               toReturn();     //按键四  返回初始位置
               break;

       }
   }
   if(isRecord && millis()-lastTime>=record_tab){  //判断是否在录制 并且间隔超过预设
        if(sampleCnt<MAXRECORD)  //判断是否录制未满
        {
             recordAngle[sampleCnt][0]=now_clawPos; //将此时的状态存入记录机中
             recordAngle[sampleCnt][1]=now_rarmPos;
             recordAngle[sampleCnt][2]=now_farmPos;
             recordAngle[sampleCnt][3]=now_basePos;
             sampleCnt++;
        }
        lastTime=millis();
   }
}



void loop_task2(int rank){
    int do1[21][2]={{},{},{},{}};
    int do2[21][2]={{},{},{},{}};
    int do3[21][2]={{},{},{},{}};
    switch(rank){
        case 0:
           for(int i=0;i<21;i++)
              servoMove(do1[i][0],do1[i][1]);
            break;
        case 1:
           for(int i=0;i<21;i++)
              servoMove(do2[i][0],do2[i][1]);
            break;
        case 2:
           for(int i=0;i<21;i++)
              servoMove(do3[i][0],do3[i][1]);
           break;
    }
    changestatus();
}

void record(){
    if(!isRecord){
        isRecord=1;
        sampleCnt=0;
        lastTime=millis();
        Serial.println("record start!");
    }
    else {
        isRecord=0;
        Serial.println("record finish!");
    }
}

void release(){
   if(!sampleCnt) Serial.println("Don't have data in record!Please record again!");
   else {
      for(int i=0;i<sampleCnt;i++)
      {
           claw.write(recordAngle[i][0]); 
           rarm.write(recordAngle[i][1]); 
           farm.write(recordAngle[i][2]); 
           base.write(recordAngle[i][3]); 
           delay(record_tab);
      }
      changestatus();
      Serial.println("record release finish!");
   }
}

void toReturn(){
   claw.write(90);
   farm.write(90);
   rarm.write(90);
   base.write(90);
}

void changestatus(){
    now_farmPos=farm.read();
    now_rarmPos=rarm.read();
    now_clawPos=claw.read();
    now_basePos=base.read();
}

void servoMove(char servoname,int toPos){
     int fromPos;
     Servo* changename;
     switch(servoname){
         case 'c':
            if(toPos>clawMax || toPos<clawMin) return;
            changename=&claw;
            fromPos=claw.read();
            now_clawPos=toPos;
            break;
         case 'b':
            if(toPos>baseMax || toPos<baseMin) return;
            changename=&base;
            fromPos=base.read();
            now_basePos=toPos;
            break;
         case 'r':
            if(toPos>rarmMax || toPos<rarmMin) return;
            changename=&rarm;
            fromPos=rarm.read();
            now_rarmPos=toPos;
            break;
         case 'f':
            if(toPos>farmMax || toPos<farmMin) return;
            changename=&farm;
            fromPos=farm.read();
            now_farmPos=toPos;
            break;
         default : return;
     }
     if(fromPos>toPos) {
         for(int i=fromPos;i>toPos;i--){
            changename->write(i);
            delay(DSD);
         } 
     }else {
         for(int i=fromPos;i<toPos;i++){
            changename->write(i);
            delay(DSD);
         }
     }

}

void joyStickPlay(){
   end_clawPos=map(judgeStill(analogRead(Rjoyy),512,30),0,1023,0,180);//获取摇杆信号  ——>claw
   end_rarmPos=map(judgeStill(analogRead(Ljoyy),512,30),0,1023,0,180);// rarm
   end_farmPos=map(judgeStill(analogRead(Rjoyx),512,30),0,1023,0,180);// farm
   end_basePos=map(judgeStill(analogRead(Ljoyx),512,30),0,1023,0,180);// base
   
   if (now_clawPos<end_clawPos) now_clawPos++;
   else if (now_clawPos>end_clawPos) now_clawPos--; claw.write(now_clawPos);

   if (now_rarmPos<end_rarmPos) now_rarmPos++;
   else if (now_rarmPos>end_rarmPos) now_rarmPos--; rarm.write(now_rarmPos);

   if (now_farmPos<end_farmPos) now_farmPos++;
   else if (now_farmPos>end_farmPos) now_farmPos--; farm.write(now_farmPos);

   if (now_basePos<end_basePos) now_basePos++;
   else if (now_basePos>end_basePos) now_basePos--; base.write(now_basePos);

   delay(10);
}

int judgeStill(int now,int center,int range)  //防中央抖动
{
    if(now>center-range && now<center+range)
    {
       return center;
    }
    return now;
}



