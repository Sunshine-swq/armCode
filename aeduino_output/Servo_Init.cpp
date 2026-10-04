#include<Arduino.h>
#include "Servo_Init.h"
#include"Variable.h"
#include"changeStatus.h"

void Servo_Init(){
    farm.attach(10);
    delay(15);
    rarm.attach(9);
    delay(15);
    base.attach(11);
    delay(15);
    claw.attach(6);
    delay(15);

}

void Servo_toReturn()
{
    Serial.println("+Command Restore Initial Position");

    int pos = base.read();
    while (pos != 90) {
        if (pos > 90) pos--;
        else pos++;
        base.write(pos);
        delay(5);
    }

    pos = claw.read();
    while (pos != 90) {
        if (pos > 90) pos--;
        else pos++;
        claw.write(pos);
        delay(5);
    }

    pos = rarm.read();
    while (pos != 90) {
        if (pos > 90) pos--;
        else pos++;
        rarm.write(pos);
        delay(5);
    }

    pos = farm.read();
    while (pos != 90) {
        if (pos > 90) pos--;
        else pos++;
        farm.write(pos);
        delay(5);
    }

    changestatus();
}

