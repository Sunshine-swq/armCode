#include <SoftwareSerial.h>

int button1=0;
int button2=1;
int button3=2;
int button4=3;
int button5=4;

int lastState[5]={HIGH,HIGH,HIGH,HIGH,HIGH};

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(button1,INPUT_PULLUP);
  pinMode(button2,INPUT_PULLUP);
  pinMode(button3,INPUT_PULLUP);
  pinMode(button4,INPUT_PULLUP);
  pinMode(button5,INPUT_PULLUP);

}

void loop() {
    // 按键1：循环夹取
    if (checkPush(2, button3)) {
        Serial.print('A');
    }
    // 按键2：录制
    if (checkPush(1, button2)) {
        Serial.print('B');
    }
    // 按键3：播放
    if (checkPush(0, button1)) {
        Serial.print('C');
    }
    // 按键4：回中
    if (checkPush(3, button4)) {
        Serial.print('D');
    }
    if (checkPush(4, button5)) {
        Serial.print('E');
    }
}

bool checkPush(int num, int pin) {  //消抖操作
    int now = digitalRead(pin);
    if (lastState[num] == HIGH && now == LOW) {
        delay(20);            // 判断消抖后再看是否还是low
        if (digitalRead(pin) == LOW) {
            lastState[num] = LOW;   //是的话就确实按下来了B
            return true;
        }
    }
    // 一直按着的数据不读取，等到按回之后读取到HIGH再改变状态
    if (now == HIGH) {  
        lastState[num] = HIGH;
    }
    return false;
}
