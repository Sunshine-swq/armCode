#include<Arduino.h>
#include<Servo.h>
#include"Servo_Init.h"
#include"Variable.h"
#include"Serial_Command.h"
#include"ServoMove.h"
#include"changeStatus.h"
#include"task2_Move.h"

void Serial_Command(){
  if(Serial.available()>0)
   {
       char getname=Serial.read(); 
       if(getname=='O' || getname=='S' ) //打开或关闭爪子
          armDataOne(getname);
       else if(getname=='H' || getname=='L') //改变速度
          speedChange(getname);
       else if(getname=='c' || getname=='r' || getname=='f' || getname=='b') //同步执行
          servoMove(getname,Serial.parseInt());    
       else if(getname=='A' || getname=='B' || getname=='C' || getname=='D' )
          task2_Move(getname);
       else {
           switch(getname){
                case 'i':
                   Servo_toReturn; //电机初始化
                   break;
                case 'k':      //显示目前各舵机的状态
                   servoStatus();
                   break;
           }
       }
   }
}

void speedChange(char getname)   //改变速度
{
     switch(getname){
         case 'H':
            upSpeed();
            break;
         case 'L':
            lowSpeed();
            break;
     }
}


void upSpeed(){   //增大速度
    if(DSD-3>3) {
        DSD-=3;
        Serial.print("Now speed is: "); Serial.println(DSD);
    }else{
      Serial.print("Error! Speed is too high! Now Speed is :");Serial.println(DSD);
    } 
}

void lowSpeed(){  //降低速度
    if(DSD+3<=30) {
        DSD+=3;
        Serial.print("Now speed is: "); Serial.println(DSD);
    }else{
      Serial.print("Error! Speed is too low! Now Speed is :");Serial.println(DSD);
    } 
}
void armDataOne(char getname) //爪子的开关
{
    switch(getname){
        case 'O':
            clawOpen();
            Serial.println("Claw is opened!");
            break;
        case 'S':
            clawClose();
            Serial.println("Claw is closed!");
            break;
    }
    changestatus();

}


void clawOpen() {    //爪子张开
    int pos = claw.read();   
    for (int i = pos; i > 0; i--) {
        claw.write(i);
        delay(DSD);
    }
}
void clawClose() {   //爪子关闭
    int pos = claw.read();
    for (int i = pos; i < 150; i++) {
        claw.write(i);
        delay(DSD);
    }
}
void servoStatus(){   //呈现各舵机状态
     Serial.println("----------------------------");
     Serial.print("clawPos :");Serial.println(now_clawPos);
     Serial.print("farmPos :");Serial.println(now_farmPos);
     Serial.print("rarmPos :");Serial.println(now_rarmPos);
     Serial.print("basePos :");Serial.println(now_basePos);
     Serial.println("");
     Serial.print("servo speed: ");Serial.println(DSD);
}