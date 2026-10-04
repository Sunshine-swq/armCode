#include<Arduino.h>
#include"Variable.h"
#include <Servo.h>

SoftwareSerial Button_link(2, 4);

Servo farm,rarm,claw,base; //设置舵机

int Ljoyx=A0;  // 左摇杆 左右 → base 底座旋转
int Ljoyy=A3;  // 左摇杆 上下 → farm 大臂俯仰
int Rjoyx=A1;  // 右摇杆 左右 → rarm 小臂
int Rjoyy=A2;  // 右摇杆 上下 → claw 爪子开合

int now_farmPos=90,now_rarmPos=90;  //舵机的初始位置
int now_clawPos=90,now_basePos=90;

int end_farmPos=90,end_rarmPos=90;  //舵机的终点位置
int end_clawPos=90,end_basePos=90;

  int baseMin,baseMax;
  int rarmMin,rarmMax;
  int farmMin,farmMax;
  int clawMin,clawMax;

int know_loop=0; //知道按键按下时执行移动哪个物体
char getCmd;    //获取指令 

int DSD=15;

#define MAXRECORD 200  //最大录制存储数
byte recordAngle[MAXRECORD][4];  //存储4个舵机的角度
int sampleCnt=0;  //录制的张数
bool isRecord=0;  //是处于录制还是处于放映
const int record_tab=75;  //每次存储的间隔75ms
int lastTime=0;
