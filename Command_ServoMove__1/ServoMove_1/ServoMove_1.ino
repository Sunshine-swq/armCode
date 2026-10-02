
#include<Servo.h>

Servo farm,claw,rarm,base;

int basePos=90,farmPos=90,clawPos=90,rarmPos=90;

const int baseMin,baseMax;
const int clawMin,clawMax;
const int rarmMin,rarmMax;
const int farmMin,farmMax;

int DSD=15;

void setup() {
  // put your setup code here, to run once:
  claw.attach(6);
  delay(200);
  base.attach(11);
  delay(200);
  rarm.attach(9);
  delay(200);
  farm.attach(10);
  delay(200);
  Serial.begin(9600);

  base.write(90);   //——>将电机处于初始状态
  delay(10);
  claw.write(90);       
  delay(10);
  rarm.write(90);
  delay(10);
  farm.write(90);
  delay(10);


}

void loop() {
  // put your main code here, to run repeatedly:
   if(Serial.available()>0)
   {
       char getname=Serial.read(); 
       if(getname=='O' || getname=='S' ) //打开或关闭爪子
          armDataOne(getname);
       else if(getname=='H' || getname=='L') //改变速度
          speedData(getname);
       else if(getname=='c' || getname=='r' || getname=='f' || getname=='b') //同步执行
          servoMove(getname);    
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
    basePos=base.read();
    clawPos=claw.read();
    rarmPos=rarm.read();
    farmPos=farm.read();

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

void speedData(char getname)   //改变速度
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


void servoMove(char getname){    // 舵机移动
    int toPos=Serial.parseInt();
    switch(getname){
        case 'b':
            if(toPos<baseMin||toPos>baseMax) return;
            base.write(toPos);
            basePos=toPos;
            break;
        case 'c':
            if(toPos<clawMin||toPos>clawMax) return;
            claw.write(toPos);
            clawPos=toPos;
            break;
        case 'r':
            if(toPos<rarmMin||toPos>rarmMax) return;
            rarm.write(toPos);
            rarmPos=toPos;
            break;
        case 'f':
            if(toPos<farmMin||toPos>farmMax) return;
            farm.write(toPos);
            farmPos=toPos;
            break;
    }
}
