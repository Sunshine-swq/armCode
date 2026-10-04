#include<Arduino.h>
#include<Servo.h>
#include"Variable.h"
#include"changeStatus.h"


void record(){
    if(!isRecord){     //状态改变  从不录制到录制
        isRecord=1;
        sampleCnt=0;   //清0录制样本
        lastTime=millis();  //录制开始时间为按下按键时间
        Serial.println("record start!");  
    }
    else {
        isRecord=0;   //状态改变  从录制到不录制
        Serial.println("record finish!");
    }
}


void Record_ing(){
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



