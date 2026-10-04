#include<Arduino.h>
#include"ServoMove.h"
#include"Variable.h"
#include <Servo.h>

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