#ifndef VARIABLE__H
#define VARIABLE__H

#include <Servo.h>
#include <Arduino.h>
#include <SoftwareSerial.h> // 必须包含这个库

    extern SoftwareSerial Button_link;
    extern SoftwareSerial JoyStick_link;

    extern Servo claw;
    extern Servo rarm;
    extern Servo farm;
    extern Servo base;

    extern int Ljoyx;
    extern int Ljoyy;  
    extern int Rjoyx;  
    extern int Rjoyy;

    extern int now_farmPos,now_rarmPos;  //舵机的初始位置
    extern int now_clawPos,now_basePos;

    extern int end_farmPos,end_rarmPos;  //舵机的终点位置
    extern int end_clawPos,end_basePos;

    extern int baseMin,baseMax;
    extern int rarmMin,rarmMax;
    extern int farmMin,farmMax;
    extern int clawMin,clawMax;

    extern int know_loop; //知道按键按下时执行移动哪个物体
    extern char getCmd;    //获取指令 

    extern int DSD;

    #define MAXRECORD 200  //最大录制存储数
    extern byte recordAngle[MAXRECORD][4];  //存储4个舵机的角度
    extern int sampleCnt;  //录制的张数
    extern bool isRecord;  //是处于录制还是处于放映
    extern const int record_tab;  //每次存储的间隔75ms
    extern int lastTime;

#endif
