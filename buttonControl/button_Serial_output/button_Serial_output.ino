

int butoon1=2;
int butoon2=3;
int butoon3=4;
int butoon4=5;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(butoon1,INPUT_PULLUP);
  pinMode(butoon2,INPUT_PULLUP);
  pinMode(butoon3,INPUT_PULLUP);
  pinMode(butoon4,INPUT_PULLUP);

}

void loop() {
    // 按键1：循环夹取
    if (checkButton(0, btn1)) {
        Serial.print('A');
    }
    // 按键2：录制
    if (checkButton(1, btn2)) {
        Serial.print('B');
    }
    // 按键3：播放
    if (checkButton(2, btn3)) {
        Serial.print('C');
    }
    // 按键4：回中
    if (checkButton(3, btn4)) {
        Serial.print('D');
    }
}

// 带消抖的按键检测：只有在“松开→按下”的瞬间返回 true
bool checkButton(int idx, int pin) {
    int now = digitalRead(pin);
    if (lastState[idx] == HIGH && now == LOW) {
        delay(20);   // 消抖
        if (digitalRead(pin) == LOW) {
            lastState[idx] = LOW;
            return true;
        }
    }
    if (now == HIGH) {
        lastState[idx] = HIGH;
    }
    return false;
}
