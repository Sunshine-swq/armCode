#include<Arduino.h>
#include "joyStick.h"
#include "Variable.h"
#include <Servo.h>

void joyStickPlay(){
 
   end_basePos=map(judgeStill(analogRead(Ljoyx),512,30),0,1023,0,180);// base
   end_farmPos=map(judgeStill(analogRead(Ljoyy),512,30),0,1023,0,180);// farm
   end_rarmPos=map(judgeStill(analogRead(Rjoyx),512,30),0,1023,0,180);// rarm
   end_clawPos=map(judgeStill(analogRead(Rjoyy),512,30),0,1023,0,180);// claw
   
   if (now_clawPos<end_clawPos) now_clawPos++;
   else if (now_clawPos>end_clawPos) now_clawPos--; claw.write(now_clawPos);

   if (now_rarmPos<end_rarmPos) now_rarmPos++;
   else if (now_rarmPos>end_rarmPos) now_rarmPos--; rarm.write(now_rarmPos);

   if (now_farmPos<end_farmPos) now_farmPos++;
   else if (now_farmPos>end_farmPos) now_farmPos--; farm.write(now_farmPos);

   if (now_basePos<end_basePos) now_basePos++;
   else if (now_basePos>end_basePos) now_basePos--; base.write(now_basePos);
   
   
   delay(10);
}
int judgeStill(int now,int center,int range)  //防中央抖动
{
    if(now>center-range && now<center+range)
    {
       return center;
    }
    return now;
}
