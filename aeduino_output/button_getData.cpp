#include<Arduino.h>
#include"Variable.h"
#include"record_release.h"
#include"Servo_Init.h"
#include "button_getData.h"
#include "changeStatus.h"
#include "ServoMove.h"


void button_Get(){
    if(Button_link.available()>0){
       getCmd=Button_link.read();
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
               Servo_toReturn();     //按键四  返回初始位置
               break;

       }
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