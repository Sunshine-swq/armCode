
#include<Servo.h>

Servo claw,farm,rarm,base;

int Ljoyx=A3;  //左侧摇杆x轴信号
int Ljoyy=A2;  //左侧摇杆y轴信号

int Rjoyx=A1;  //右侧摇杆x轴信号
int Rjoyy=A0;  //右侧摇杆y轴信号



void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  claw.attach(6);
  rarm.attach(9);
  farm.attach(10);
  base.attach(11);

}

void loop() {
  // put your main code here, to run repeatedly:
   int baseServo=analogRead(Ljoyx);//获取摇杆信号
   int rarmServo=analogRead(Ljoyy);
   int farmServo=analogRead(Rjoyx);
   int clawServo=analogRead(Rjoyy);

   base.write(map(baseServo,0,1023,0,180));  //控制舵机转动
   rarm.write(map(rarmServo,0,1023,0,180));
   farm.write(map(farmServo,0,1023,0,180));
   claw.write(map(clawServo,0,1023,0,180));

   delay(15);



}
