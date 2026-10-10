
#include<Servo.h>

Servo farm,rarm,claw,base; //设置舵机

int joyLastMs=0;
bool laseStatus=1;
bool nowStatus=1;
bool ControlMode=0;

int Ljoyx=A0;  // 左摇杆 左右 → base 底座旋转
int Ljoyy=A1;  // 左摇杆 上下 → farm 大臂俯仰
int Rjoyx=A2;  // 右摇杆 左右 → rarm 小臂
int Rjoyy=A3;  // 右摇杆 上下 → claw 爪子开合

int now_farmPos=90,now_rarmPos=90;  //舵机的初始位置
int now_clawPos=90,now_basePos=90;

int end_farmPos=90,end_rarmPos=90;  //舵机的终点位置
int end_clawPos=90,end_basePos=90;

  int baseMin=5,baseMax=175;
  int rarmMin=5,rarmMax=175;
  int farmMin=5,farmMax=175;
  int clawMin=5,clawMax=180;

int know_loop=0; //知道按键按下时执行移动哪个物体
char getCmd;    //获取指令 

int DSD=15;

unsigned long joylastMs=0;

#define MAXRECORD 173  //最大录制存储数 13s
byte recordAngle[MAXRECORD][4];  //存储4个舵机的角度
int sampleCnt=0;  //录制的张数
bool isRecord=0;  //是处于录制还是处于放映
const int record_tab=75;  //每次存储的间隔75ms
int lastTime=0;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(5,INPUT_PULLUP);
  pinMode(4,INPUT_PULLUP);
  Servo_Init(); //舵机配置
  Servo_toReturn(); //舵机返回初始状态
  delay(500);

}

void loop() {
  // put your main code here, to run repeatedly:
   judgeMode();
   if(!ControlMode) Serial_Command(); //手柄操作
   else button_Get();   //按键操作

   
   joyStickPlay();
   Record_ing();  //判断是否记录：开始记录
  
}

void judgeMode(){
    nowStatus=digitalRead(5);
    if(laseStatus==HIGH&&nowStatus==LOW){
        ControlMode=0;
        nowStatus=HIGH;
        Serial.println(F("Now is JoyStick Mode!"));
        while(!digitalRead(5));
    }
    nowStatus=digitalRead(4);
    if(laseStatus==HIGH&&nowStatus==LOW){
        ControlMode=1;
        nowStatus=HIGH;
        Serial.println(F("Now is Button Mode!"));
        while(!digitalRead(4));
        know_loop=0;
    }
}


void Serial_Command(){
  if(Serial.available()>0)
   {
       char getname=Serial.read(); 
       if(getname=='O' || getname=='S' ) //打开或关闭爪子
          armDataOne(getname);
       else if(getname=='H' || getname=='L') //改变速度
          speedChange(getname);
       else if(getname=='c' || getname=='r' || getname=='f' || getname=='b') //同步执行
       {
           int data=Serial.parseInt();
           servoMove(getname,data); 
       }
             
       else if(getname=='A' || getname=='B' || getname=='C' || getname=='D' )
          task2_Move(getname);
       else {
           switch(getname){
                case 'i':
                   Servo_toReturn(); //电机初始化
                   break;
                case 'k':      //显示目前各舵机的状态
                   servoStatus();
                   break;
                case 'x':
                   multiServoMove();
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
    if(DSD-3>=3) {
        DSD-=3;
        Serial.print(F("Now speed is: ")); Serial.println(DSD);
    }else{
      Serial.print(F("Error! Speed is too high! Now Speed is :"));Serial.println(DSD);
    } 
}

void lowSpeed(){  //降低速度
    if(DSD+3<=30) {
        DSD+=3;
        Serial.print(F("Now speed is: ")); Serial.println(DSD);
    }else{
      Serial.print(F("Error! Speed is too low! Now Speed is :"));Serial.println(DSD);
    } 
}
void armDataOne(char getname) //爪子的开关
{
    switch(getname){
        case 'O':
            clawOpen();
            Serial.println(F("Claw is opened!"));
            break;
        case 'S':
            clawClose();
            Serial.println(F("Claw is closed!"));
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
     Serial.println(F("----------------------------"));
     Serial.print(F("clawPos :"));Serial.println(now_clawPos);
     Serial.print(F("farmPos :"));Serial.println(now_farmPos);
     Serial.print(F("rarmPos :"));Serial.println(now_rarmPos);
     Serial.print(F("basePos :"));Serial.println(now_basePos);
     Serial.println();
     Serial.print(F("servo speed: "));Serial.println(DSD);
}

void multiServoMove(){
    int x=Serial.parseInt();
    int y=Serial.parseInt();
    int z=Serial.parseInt();

    x=constrain(x,baseMin,baseMax);
    y=constrain(y,farmMin,farmMax);
    z=constrain(z,rarmMin,rarmMax);
    
    int stepsMax=max(max(abs(x-now_basePos),abs(y-now_farmPos)),abs(z-now_rarmPos));

    if(!stepsMax) return;
    for(int i=1;i<stepsMax;i++){
        base.write(now_basePos+(x-now_basePos)*i/stepsMax);
        farm.write(now_farmPos+(y-now_farmPos)*i/stepsMax);
        rarm.write(now_rarmPos+(z-now_rarmPos)*i/stepsMax);
        delay(DSD);
    }
    base.write(x);
    farm.write(y);
    rarm.write(z);
    changestatus();
}

int prebase=90,prefarm=90,prerarm=90,preclaw=90;

void joyStickPlay(){
    if(millis()-joyLastMs<15) return;
    joyLastMs=millis();

    int tb=ReadAndJudge(Ljoyx,prebase); //防止悬空  放中央抖动  放微小震动
    int ts=ReadAndJudge(Ljoyy,prefarm);
    int te=ReadAndJudge(Rjoyx,prerarm);
    int tc=ReadAndJudge(Rjoyy,preclaw);
    
    if(tb!=-1){
      prebase=tb;
      end_basePos=constrain(prebase,baseMin,baseMax);//避免超过极限
      if (now_basePos<end_basePos) now_basePos++;
      else if (now_basePos>end_basePos) now_basePos--; 
      base.write(now_basePos);
    }

    if(ts!=-1){
      prefarm=ts;
      end_farmPos=constrain(prefarm,farmMin,farmMax);//避免超过极限
      if (now_farmPos<end_farmPos) now_farmPos++;
      else if (now_farmPos>end_farmPos) now_farmPos--; 
      farm.write(now_farmPos);
    }
    if(te!=-1){
      prerarm=te;
      end_rarmPos=constrain(prerarm,rarmMin,rarmMax);//避免超过极限
      if (now_rarmPos<end_rarmPos) now_rarmPos++;
      else if (now_rarmPos>end_rarmPos) now_rarmPos--;
      rarm.write(now_rarmPos);
    }
    if(tc!=-1){
      preclaw=tc;
      end_clawPos=constrain(preclaw,clawMin,clawMax);//避免超过极限
      if (now_clawPos<end_clawPos) now_clawPos++;
      else if (now_clawPos>end_clawPos) now_clawPos--; 
      claw.write(now_clawPos);
    }
    delay(DSD);
}


int ReadAndJudge(int pin,int last){  //防止在未连接摇杆时引脚悬空发生抖动
   int p1=analogRead(pin);  //读取此刻瞬间的引脚的两个值
   int p2=analogRead(pin);
   if(abs(p1-p2)>8) return last;
   int average=(p1+p2)/2;  //如果两者一直在中央进行抖动，那保持静止 不太可能发生一瞬间从一端到另一端；
   if(average>512-30 && average<512+30) return -1;
   int pos=map(average,0,1023,5,175);
   int chazhi=(pos>last)?(pos-last):(last-pos);
   if(chazhi<4) return last;  //微小振动忽略
   return pos;
}

void servoMove(char servoname,int toPos){
     int fromPos;
     Servo *changename;
     switch(servoname){
         case 'c':
            if(toPos>clawMax || toPos<clawMin) return;
            changename=&claw;
            fromPos=claw.read();
            now_clawPos=toPos;
            break;
         case 'b':
            if(toPos>baseMax || toPos<baseMin) return;
            changename=&base;
            fromPos=base.read();
            now_basePos=toPos;
            break;
         case 'r':
            if(toPos>rarmMax || toPos<rarmMin) return;
            changename=&rarm;
            fromPos=rarm.read();
            now_rarmPos=toPos;
            break;
         case 'f':
            if(toPos>farmMax || toPos<farmMin) return;
            changename=&farm;
            fromPos=farm.read();
            now_farmPos=toPos;
            break;
         default : return;
     }
     if(fromPos>toPos) {
         for(int i=fromPos;i>toPos;i--){
            changename->write(i);
            delay(DSD);
         } 
     }else {
         for(int i=fromPos;i<toPos;i++){
            changename->write(i);
            delay(DSD);
         }
     }

}
void Servo_Init(){
    farm.attach(7);
    delay(15);
    rarm.attach(8);
    delay(15);
    base.attach(9);
    delay(15);
    claw.attach(6);
    delay(15);

}

void Servo_toReturn()
{
    Serial.println(F("+Command Restore Initial Position"));

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

void record(){
    if(!isRecord){     //状态改变  从不录制到录制
        isRecord=1;
        sampleCnt=0;   //清0录制样本
        lastTime=millis();  //录制开始时间为按下按键时间
        Serial.println(F("record start!"));  
    }
    else {
        isRecord=0;   //状态改变  从录制到不录制
        Serial.println(F("record finish!"));
    }
}


void Record_ing(){
    if(isRecord && millis()-lastTime>=record_tab){  //判断是否在录制 并且间隔超过预设
        if(sampleCnt<MAXRECORD)  //判断是否录制未满
        {
             recordAngle[sampleCnt][0]=now_clawPos; //将此时的状态存入记录机中
             recordAngle[sampleCnt][1]=now_rarmPos;
             recordAngle[sampleCnt][2]=now_farmPos;
             recordAngle[sampleCnt][3]=now_basePos;
             sampleCnt++;
        }
        lastTime=millis();
   }
}


void release(){
   if(!sampleCnt) Serial.println(F("Don't have data in record!Please record again!"));
   else {
      for(int i=0;i<sampleCnt;i++)
      {
           claw.write(recordAngle[i][0]); 
           rarm.write(recordAngle[i][1]); 
           farm.write(recordAngle[i][2]); 
           base.write(recordAngle[i][3]); 
           delay(record_tab);
      }
      changestatus();
      Serial.println(F("record release finish!"));
   }
}
void changestatus(){
    now_farmPos=farm.read();
    now_rarmPos=rarm.read();
    now_clawPos=claw.read();
    now_basePos=base.read();
}
void button_Get(){
    if(Serial.available()>0){
       getCmd=Serial.read();
       switch(getCmd){
           case 'A':
               loop_task2(know_loop%3);  //按键一  循环任务2
               know_loop++;
               break;
           case 'B':
               record();      //按键二  录制
               break;
           case 'C':
               release();     //按键三  回放
               break;
           case 'D':
               Servo_toReturn();     //按键四  返回初始位置
               break;

       }
    }
}
void loop_task2(int rank){
    int do1[21][2]={{'c',50},{'c',115},{'c',60},{'c',30},{'c',45},{'c',10},{'c',90}};
    int do2[21][2]={{'b',50},{'b',115},{'b',60},{'b',30},{'b',45},{'b',10},{'b',90}};
    int do3[21][2]={{'r',50},{'r',115},{'r',60},{'r',30},{'r',45},{'r',10},{'r',90}};
    switch(rank){
        case 0:
           for(int i=0;i<21;i++)
              servoMove(do1[i][0],do1[i][1]);
            break;
        case 1:
           for(int i=0;i<21;i++)
              servoMove(do2[i][0],do2[i][1]);
            break;
        case 2:
           for(int i=0;i<21;i++)
              servoMove(do3[i][0],do3[i][1]);
           break;
    }
    changestatus();
}
void task2_Move(char num)
{
    int do1[7][2]={{'b',50},{'c',115},{'c',60},{'c',30},{'c',45},{'c',10},{'c',90}};
    int do2[21][2]={{},{},{},{}};
    int do3[21][2]={{},{},{},{}};
    switch(num){
        case 'A':
           for(int i=0;i<7;i++) //注意循环次数要与上面对应 不然会出现多次进行A操作的情况
           {
              servoMove(do1[i][0],do1[i][1]);
              delay(100);
           }   
            break;
        case 'B':
           for(int i=0;i<21;i++)
              servoMove(do2[i][0],do2[i][1]);
            break;
        case 'C':
           for(int i=0;i<21;i++)
              servoMove(do1[i][0],do1[i][1]);
           break;
    }
    changestatus();
}


