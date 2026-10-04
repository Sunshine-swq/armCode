#include<Arduino.h>
#include<Servo.h>
#include"ServoMove.h"
#include"changeStatus.h"


void task2_Move(char num)
{
    int do1[21][2]={{},{},{},{}};
    int do2[21][2]={{},{},{},{}};
    int do3[21][2]={{},{},{},{}};
    switch(num){
        case 'A':
           for(int i=0;i<21;i++)
              servoMove(do1[i][0],do1[i][1]);
            break;
        case 'B':
           for(int i=0;i<21;i++)
              servoMove(do2[i][0],do2[i][1]);
            break;
        case 'C':
           for(int i=0;i<21;i++)
              servoMove(do3[i][0],do3[i][1]);
           break;
    }
    changestatus();
}

