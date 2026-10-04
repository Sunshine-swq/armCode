#include<Arduino.h>
#include<Servo.h>
#include"Variable.h"


void changestatus(){
    now_farmPos=farm.read();
    now_rarmPos=rarm.read();
    now_clawPos=claw.read();
    now_basePos=base.read();
}
