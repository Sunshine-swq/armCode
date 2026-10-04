#include<Servo.h>
#include <SoftwareSerial.h>
#include "joyStick.h"
#include "Servo_Init.h"
#include "ServoMove.h"
#include "Variable.h"
#include "record_release.h"
#include "changeStatus.h"
#include "button_getData.h"
#include "Serial_Command.h"

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Button_link.begin(9600);
  Servo_Init(); //舵机配置
  Servo_toReturn(); //舵机返回初始状态
  delay(500);

}

void loop() {
  // put your main code here, to run repeatedly:
   joyStickPlay(); //手柄操作
   button_Get();   //按键操作
   Record_ing();  //判断是否记录：开始记录
   Serial_Command();
}


