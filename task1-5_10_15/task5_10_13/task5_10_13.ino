#include<Servo.h>

typedef struct Pose{
    float x, y, z;
};       //一个空间三个坐标（x 左右, y 前后, z 高低）

#define BTN_START     8       //开始画
#define BTN_PAUSE     4       //  暂停
#define BTN_RESUME    5       //  继续
#define BTN_CANCEL    2       //  取消
#define BTN_HOME      3       //回待机
//__________________________________________________________________________________模式



//___________________________________________________________________________________舵机
Servo farm,rarm,claw,base; //设置舵机

#define baseMin 0//舵机极限最大角和最小角
#define baseMax 180
#define rarmMin 0 
#define rarmMax 180
#define farmMin 0
#define farmMax 180 
#define clawMin 0
#define clawMax 180


//—————————————————————————————————————————————————————————————————————————————————————摇杆
int Ljoyx=A0;  // 左摇杆 左右 → base 底座旋转
int Ljoyy=A1;  // 左摇杆 上下 → farm 大臂俯仰
int Rjoyx=A2;  // 右摇杆 左右 → rarm 小臂
int Rjoyy=A3;  // 右摇杆 上下 → claw 爪子开合

byte now_farmPos=180,now_rarmPos=90;  //舵机的初始位置
byte now_clawPos=90,now_basePos=90;  

byte end_farmPos=180,end_rarmPos=90;  //舵机的终点位置
byte end_clawPos=90,end_basePos=90;
int prebase=90,prefarm=90,prerarm=90,preclaw=90;   //摇杆所需数据
uint32_t joyLastMs=0;

//___________________________________________________________________________________Serial_cmd
int know_loop=0; //知道按键按下时执行移动哪个物体
char getCmd;    //获取指令 
byte DSD=15;//改变速度的

//————————————————————————————————————————————————————————————————————————————————————录制

#define MAXRECORD 200  //最大录制存储数
byte recordAngle[MAXRECORD][4];  //存储4个舵机的角度
int sampleCnt=0;  //录制的张数
bool isRecord=0;  //是处于录制还是处于放映
const int record_tab=50;  //每次存储的间隔75ms
int lastTime=0;

//————————————————————————————————————————————————————————————————————————————————————任务五

//机械的尺寸大小
#define BASE_H      0.0 //底座转中心与大臂底部轴心距离
#define SHOULDER_H  55.0//大臂底部到地面距离
#define L1          80.0//大臂的长度  ——> 大臂舵机到小臂轴心
#define L2          95.0//小臂轴心到爪子夹笔的位置
#define PEN_H       60.0//爪子夹笔位置到笔尖位置
#define PEN_DX      0.0//笔尖相对安转位置的水平偏移  向前为正
#define PEN_DY      0.0//笔尖相对安转位置的水平位移   向右为正

//舵机角和几何角的关系
#define BS_A   90.0  //底座：几何角为 0° 时，舵机应该写多少度（零点偏移）
#define BS_B    1.0  //底座：几何角每增加 1°，舵机要跟着变多少度（负号 = 反向）
#define SH_A   90.0  //大臂(farm)：零点偏移
#define SH_B    1.0  //大臂(farm)：倍率兼方向
#define EL_A   90.0  //小臂(rarm)：零点偏移
#define EL_B   -1.0  //小臂（rarm）：倍率兼方向
#define Z_PAPER       0.0     // 落笔高度：笔尖贴到纸面(z=0 就是纸面)
#define Z_SAFE       35.0     // 抬笔高度：笔尖离纸 35mm。

//绘画的精度和速度
#define Step_Long       1.5     //步长(mm)：每步走多长 同时决定了一段走多少步
#define Step_Time      22      //每步的间隔：决定快慢
#define Step_Time_Min   10     //最快多快
#define Step_Time_Max   60     //最慢多慢
#define Speed_DSD   4      //一次加速 减速 改几毫秒

//图形的位置
#define Home_X      90.0  //待机点 笔尖的位置xy
#define Home_Y   0.0   
#define Home_Z   80.0  //笔尖高度
#define Shape_CX      95.0    //图形中心x
#define Shape_CY       0.0    //图形中心 y
#define Shape_S       30.0    //图形半边长;边长=SHAPE_S×2=60mm
#define Line_X1       60.0    //直线的起点x
#define Line_X2      140.0    //直线的终点 x 长度 80mm
#define Line_Y         0.0    //直线所在的 y

#define Claw_Strech   90       //绘图时夹爪保持的角度
#define D2R  (PI/180.0)       //度 ->弧度 的换算系数
#define R2D  (180.0/PI)       //弧度 -> 度 的换算系数

#define Max_sPoint  20  //舵点的个数  也就是一条线分为几个点
#define Max_SEG   (Max_sPoint-1)  // 这条线分为几段

//————————————————————————————————————————————————————————————————————————————————————任务五 数据结构（Pose 结构体已在文件顶部定义）
Pose sPoint[Max_sPoint];                  //航点坐标表（绘制用的点序列）
uint16_t sSteps[Max_SEG];                 //每一段要切成几小步
byte  spCount = 0;                        //当前经过了几个航点

//判断状态
enum DrawState {Wait,Teach,Draw,Pause,Done}; //各种状态
byte  servoStates = Wait;                 //一开时为待机状态
uint16_t now_duan = 0, now_step = 0;      //当前画到第几段第几步
uint32_t lastStepTime = 0;                //上一步执行时间 millis()


  bool laseStatus=1;
  bool nowStatus1=1;
  bool nowStatus2=1;
  bool ControlMode=0;

void judgeMode(){
    nowStatus1=digitalRead(4);
    if(laseStatus==HIGH&&nowStatus1==LOW){
        if(ControlMode!=1){
            ControlMode=1;
            nowStatus1=HIGH;
            Serial.println(F("Now is Task4_Button Mode!"));
            while(!digitalRead(4));
            know_loop=0;
        }
        else if(ControlMode!=0){
            ControlMode=0;
            nowStatus1=HIGH;
            Serial.println(F("Now is SerialCommand Mode!"));
            while(!digitalRead(4));
        }
    }
    nowStatus2=digitalRead(5);
    if(laseStatus==HIGH&&nowStatus2==LOW){
        ControlMode=2;
        nowStatus2=HIGH;
        Serial.println(F("Now is Task5_Button Mode!"));
        while(!digitalRead(5));
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
        Serial.print(F("Now speed is: "));  Serial.println(DSD);
    }else{
      Serial.print(F("Error! Speed is too high! Now Speed is :"));  Serial.println(DSD);
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

    x=limitPos(x,baseMin,baseMax);
    y=limitPos(y,farmMin,farmMax);
    z=limitPos(z,rarmMin,rarmMax);
    
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


void joyStickPlay(){
    if(millis()-joyLastMs<15) return;
    joyLastMs=millis();

    int tbase=ReadAndJudge(Ljoyx,prebase); //防止悬空  放中央抖动  放微小震动
    int tfarm=ReadAndJudge(Ljoyy,prefarm);
    int trarm=ReadAndJudge(Rjoyx,prerarm);
    int tclaw=ReadAndJudge(Rjoyy,preclaw);
    
    if(tbase!=-1){
      prebase=tbase;
      end_basePos=limitPos(prebase,baseMin,baseMax);//避免超过极限
      if (now_basePos<end_basePos) now_basePos++;
      else if (now_basePos>end_basePos) now_basePos--; 
      base.write(now_basePos);
    }

    if(tfarm!=-1){
      prefarm=tfarm;
      end_farmPos=limitPos(prefarm,farmMin,farmMax);//避免超过极限
      if (now_farmPos<end_farmPos) now_farmPos++;
      else if (now_farmPos>end_farmPos) now_farmPos--; 
      farm.write(now_farmPos);
    }
    if(trarm!=-1){
      prerarm=trarm;
      end_rarmPos=limitPos(prerarm,rarmMin,rarmMax);//避免超过极限
      if (now_rarmPos<end_rarmPos) now_rarmPos++;
      else if (now_rarmPos>end_rarmPos) now_rarmPos--;
      rarm.write(now_rarmPos);
    }
    if(tclaw!=-1){
      preclaw=tclaw;
      end_clawPos=limitPos(preclaw,clawMin,clawMax);//避免超过极限
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
   int pos=update_Pos(average,0,1023,0,180);
   int chazhi=abs(pos-last);
   if(chazhi<4) return last;  //微小振动忽略
   return pos;
}

int update_Pos(int Pos,int before_min,int before_max,int now_min,int now_max){
     return (Pos-before_min)/(before_max-before_min)*(now_max-now_min)+now_min;
}
int limitPos(int Pos,int min,int max){
    if(Pos>max) Pos=max;
    if(Pos<min) Pos=min;
    return Pos;
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
    int pos=0;
    
    pos = claw.read();
    while (pos != 28
    ) {
        if (pos > 28) pos--;
        else pos++;
        claw.write(pos);
        delay(DSD);
    }

    pos = rarm.read();
    while (pos != 90) {
        if (pos > 90) pos--;
        else pos++;
        rarm.write(pos);
        delay(DSD);
    }

    pos = farm.read();
    while (pos != 180) {
        if (pos > 180) pos--;
        else pos++;
        farm.write(pos);
        delay(DSD);
    }
    pos = base.read();
    while (pos != 90) {
        if (pos > 90) pos--;
        else pos++;
        base.write(pos);
        delay(DSD);
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
    int do1[11][2]={{'b',90},{'c',9},{'f',170},{'r',110},{'c',110},{'r',79},{'f',174},{'b',130},{'f',140},{'r',140},{'c',9}};
    int do2[11][2]={{'b',40},{'c',9},{'f',170},{'r',110},{'c',110},{'r',79},{'f',174},{'b',90},{'f',140},{'r',140},{'c',9}};
    int do3[11][2]={{'b',140},{'c',9},{'f',170},{'r',110},{'c',110},{'r',79},{'f',174},{'b',40},{'f',140},{'r',140},{'c',9}};
    switch(rank){
        case 0:
           for(int i=0;i<11;i++)
              servoMove(do1[i][0],do1[i][1]);
              Serial.println(F("record release finish!"));
            break;
        case 1:
           for(int i=0;i<11;i++)
              servoMove(do2[i][0],do2[i][1]);
            break;
        case 2:
           for(int i=0;i<11;i++)
              servoMove(do3[i][0],do3[i][1]);
           break;
    }
    changestatus();
}
void task2_Move(char num)
{
    int do1[11][2]={{'b',90},{'c',9},{'f',170},{'r',110},{'c',110},{'r',79},{'f',174},{'b',130},{'f',140},{'r',140},{'c',9}};
    int do2[11][2]={{'b',40},{'c',9},{'f',170},{'r',110},{'c',110},{'r',79},{'f',174},{'b',90},{'f',140},{'r',140},{'c',9}};
    int do3[11][2]={{'b',140},{'c',9},{'f',170},{'r',110},{'c',110},{'r',79},{'f',174},{'b',40},{'f',140},{'r',140},{'c',9}};
    switch(num){
        case 'A':
           for(int i=0;i<11;i++) //注意循环次数要与上面对应 不然会出现多次进行A操作的情况
           {
              servoMove(do1[i][0],do1[i][1]);
              delay(100);
           }
            Servo_toReturn();  
            break;
        case 'B':
           for(int i=0;i<11;i++){
              servoMove(do2[i][0],do2[i][1]);
              delay(100);
           }
              
            Servo_toReturn(); 
            break;
        case 'C':
           for(int i=0;i<11;i++){
              servoMove(do3[i][0],do3[i][1]);
              delay(100);
           }
             
           Servo_toReturn(); 
           break;
    }
    changestatus();
}

//————————————————————————————————————————————————————————————————————————————task5___绘图工具
float limitFloat(float num, float low, float high){            //限制
    if(num<low) num=low;
    else if(num>high) num=high;
    return num;
}
int FiveUpFourLow(float num){   //四舍五入
    if(num>=0) num+=0.5;
    else num-=0.5;
    return (int)num;
}
void report(const __FlashStringHelper* s){
    Serial.println(s);
}
Pose getPose(float x, float y, float z){
    Pose p;
    p.x = x;
    p.y = y; 
    p.z = z; 
    return p; 
}

//____________________________________________________________________________task5____绘图计算



//大写且几何角  小写为舵机角
//根据角度求得坐标的正解和根据坐标求得角度的逆解

//由几何角获得坐标

Pose FK(float baseAngle, float farmAngle, float rarmAngle){ 
  float rbaseAngle=baseAngle*D2R,rfarmAngle=farmAngle*D2R,rrarmAngle=rarmAngle*D2R;    //首先这些角转化为弧度制，因为math只认弧度
  float r=BASE_H+L1*cos(rfarmAngle)+L2*cos(rfarmAngle+rrarmAngle); //然后根据公式算出底座到达笔尖的水平距离
  float z=SHOULDER_H+L1*sin(rfarmAngle)+L2*sin(rfarmAngle+rrarmAngle); //这个是得到笔尖的高
  Pose p; 
  p.x=r*cos(rbaseAngle); //根据底座的角 得到笔尖的xy坐标
  p.y=r*sin(rbaseAngle);
  p.z=z;       
  p.x-=PEN_DX;  //因为算的是到爪子夹住笔的距离，所以在这里应该加上笔的偏移量，比如笔尖的位置应该在z的基础上下降PEN_H毫米
  p.y-=PEN_DY;
  p.z-=PEN_H;
  return p;
}

//有坐标获得几何角
bool IK(Pose p,float &baseAngle,float &farmAngle,float &rarmAngle) { //一个函数难以同时返回三个值，所以这里直接取地址改实际值
  p.x+=PEN_DX;  //因为要计算实际角度 所以应该回归到夹子夹住笔的位置
  p.y+=PEN_DY;
  p.z+=PEN_H;  
  float r=sqrt(p.x*p.x+p.y*p.y)-BASE_H;  //计算出笔尖到大臂底部的水平距离
  float z=p.z-SHOULDER_H;    //再计算出爪子首端到大臂底部的所在水平面的高度
  float d2=r*r+z*z;     
  float d=sqrt(d2);      //计算出爪子到大臂底部的距离
  float dmax=(L1+L2)-0.5;    //计算出最大和最小的延伸距离
  float dmin=fabs(L1-L2) + 0.5;  
  if (d>dmax||d<dmin) return false;    //超过极限就停 返回0         
  float crrarmAngle=limitFloat((d2-L1*L1-L2*L2)/(2.0*L1*L2),-1.0,1.0); //用余弦定理求小臂与大臂延长线所形成角的余弦值
  float rrarmAngle=(-1)*acos(crrarmAngle);  //解出余弦角  因为是机械臂向下的  所以要变成负数 
  float rfarmAngle=atan2(z,r)-atan2(L2*sin(rrarmAngle),L1+L2*cos(rrarmAngle));  //然后求得爪子到大臂底端和大臂形成的夹角 再求得小臂与大臂延长线所形成的角(是负的，因为角是负数)  两者相减 便求得出大臂角度
  baseAngle=atan2(p.y,p.x)*R2D;  //底座的角      
  farmAngle=rfarmAngle*R2D;                //大臂的角
  rarmAngle=rrarmAngle*R2D;                //小臂的角
  return true;                       
}


//舵机和几何角的换算

//几何角——>舵机角
void mathToservo(float baseAngle, float farmAngle, float rarmAngle) {
  // 底座
  int baseangle=limitPos(FiveUpFourLow(BS_A+BS_B*baseAngle),baseMin,baseMax);
  //大臂
  int farmangle=limitPos(FiveUpFourLow(SH_A+SH_B*farmAngle),farmMin,farmMax);
  //小臂
  int rarmangle=limitPos(FiveUpFourLow(EL_A+EL_B*rarmAngle),rarmMin,rarmMax);

  base.write(baseangle); farm.write(farmangle); rarm.write(rarmangle);
}

// 舵机角到几何角
void servoTomath(int baseangle, int farmangle, int rarmangle, float &baseAngle, float &farmAngle, float &rarmAngle) {
  
  if(BS_B!=0)  baseAngle=(baseangle-BS_A)/BS_B;
  else  baseAngle=0;
  if(SH_B!=0)  farmAngle=(farmangle-SH_A)/SH_B;
  else  farmAngle=0;
  if(EL_B!=0)  rarmAngle=(rarmangle-EL_A)/EL_B;
  else  rarmAngle=0;
}

void clear_sPoint(){  //清空舵点表
    spCount=0;
}

void add_sPoint(float x,float y,float z){  //在舵点表增加一个点
  if(spCount<Max_sPoint){
      sPoint[spCount].x=x;
      sPoint[spCount].y=y;
      sPoint[spCount].z=z;
  }
}

bool beforeCheck(){   //提前检测是否每个点都能够到达
    float baseAngle,farmAngle,rarmAngle;
    for(int i=0;i<spCount;i++){
       if(!IK(sPoint[i],baseAngle,farmAngle,rarmAngle))return false;
    }
    return true;
} 

void stepsum() //获得每段的
{   
    for(int i=0;i<spCount-1;i++){
        float dx=sPoint[i+1].x-sPoint[i].x,dy=sPoint[i+1].y-sPoint[i].y,dz=sPoint[i+1].z-sPoint[i].z;
        float d=sqrt(dx*dx+dy*dy+dz*dz);
        int n=(int)(d/Step_Long+0.9999);
        if(n<1) n=1;
        if(n>500) n=500;
        sSteps[i]=(uint16_t)n;
    }
    now_duan=0;
    now_step=0;
}

void getMiddlePoint(uint16_t now_duan,float jindu,float &x,float &y,float &z){ //求第now_duan在某进度下的坐标；
    x=sPoint[now_duan].x+(sPoint[now_duan+1].x-sPoint[now_duan].x)*jindu;
    y=sPoint[now_duan].y+(sPoint[now_duan+1].y-sPoint[now_duan].y)*jindu;
    z=sPoint[now_duan].z+(sPoint[now_duan+1].z-sPoint[now_duan].z)*jindu;            
}
//——————————————————————————————————————————————————————————————————————————————————————task5_绘图移动
void gotoPose(Pose p) {                       //到达某一坐标点
    float baseAngle,farmAngle,rarmAngle;
    if (IK(p,baseAngle,farmAngle,rarmAngle)){
    mathToservo(baseAngle, farmAngle, rarmAngle);

    prebase=limitPos(FiveUpFourLow(BS_A+BS_B*baseAngle),baseMin,baseMax); //摇杆记录当前位置 防止摇杆
    prefarm=limitPos(FiveUpFourLow(SH_A+SH_B*farmAngle),farmMin,farmMax);
    prerarm=limitPos(FiveUpFourLow(EL_A+EL_B*rarmAngle),rarmMin,rarmMax);
  } else {
    report(F("goal unreachable"));
  }
}   

//——————————————————————————————————————————————————————————————————————————————————————task5_绘图舵点的设置
void planLine(){                               //画直线
    clear_sPoint();
    add_sPoint(Home_X,Home_Y,Home_Z);
    add_sPoint(Line_X1,Line_Y,Z_SAFE);
    add_sPoint(Line_X1,Line_Y,Z_PAPER);
    add_sPoint(Line_X2,Line_Y,Z_PAPER);
    add_sPoint(Line_X2,Line_Y,Z_SAFE);
    add_sPoint(Home_X,Home_Y,Home_Z);
    stepsum();
}
void planCharacter(){                          //画图形
    clear_sPoint();
    float Left_edge=Shape_CX-Shape_S;   //左边界
    float Right_edge=Shape_CX+Shape_S;  //右边界
    float Base_edge=Shape_CY-Shape_S;   //下边界
    float Top_edge=Shape_CY+Shape_S;    //上边界
    add_sPoint(Home_X,Home_Y,Home_Z);   //先处于初始位置
    add_sPoint(Left_edge,Base_edge,Z_SAFE);  //抬笔放在左下角开始
    add_sPoint(Left_edge,Base_edge,Z_PAPER); //落笔
    
    //绘画N
    add_sPoint(Left_edge,Top_edge,Z_PAPER);   //从左下角到右上角
    add_sPoint(Right_edge,Base_edge,Z_PAPER);  //从左上角到右下角
    add_sPoint(Right_edge,Top_edge,Z_PAPER);   //从右下角到右上角
    add_sPoint(Right_edge,Top_edge,Z_SAFE);
    add_sPoint(Home_X,Home_Y,Home_Z);

    stepsum(); //计算上面那些点各点之间所需要的段数
}
//——————————————————————————————————————————————————————————————————————————————————————task5_绘图状态的设置函数
void stopDraw(const __FlashStringHelper* problem){               //停止绘画
    servoStates=Wait;
    report(problem);
}
void drawTick(){                                                 //持续绘画——小步执行
    if(servoStates!=Draw) return;
    if(millis()-lastStepTime<Step_Time)  return;
    lastStepTime=millis();
    if(now_duan+1>=spCount){
        stopDraw(F("Draw is done!"));
        return ;
    }
    float x,y,z;
    getMiddlePoint(now_duan,(float)now_step/(float)sSteps[now_duan],x,y,z);
    Pose p;
    p.x=x;
    p.y=y;
    p.z=z;
    float baseAngle,farmAngle,rarmAngle;
    if(!IK(p,baseAngle,farmAngle,rarmAngle)){
        stopDraw(F("I can not get there!"));
        return;
    }
    mathToservo(baseAngle,farmAngle,rarmAngle);
    now_step++;

    if(now_step>sSteps[now_duan]){
        now_step=0;
        now_duan++;
    }
    if(now_duan+1>=spCount) stopDraw(F("Draw is done!"));
}
void drawing(){                                                  //正在绘画
    claw.write(Claw_Strech);
    if(!beforeCheck()){
          report(F("I can not stretch!"));
          return;
    }
    servoStates=Draw;
    lastStepTime=millis();
}
void doPause()                                                   //绘画暂停
{
    if (servoStates==Draw)
    { 
        servoStates =Pause; 
        report(F("Pause press button could proceed!"));
    }
}
void doContinue(){                                               //绘画继续
    if(servoStates==Pause){
        servoStates=Draw;
        lastStepTime=millis();
        report(F("Drawing continue!"));
    }
}
void doConcel(){                                                 //绘画取消并回起始点
    if(servoStates==Draw||servoStates ==Pause || servoStates==Wait){
        servoStates=Wait;
        gotoPose(getPose(Home_X, Home_Y, Home_Z));
    }
}


//————————————————————————————————————————————————————————————————————————————————task5_接收数据并执行
void getBottonTask5_Data(){
    if(Serial.available()>0){
        char getname=Serial.read();
        switch(getname){
            case 'A': 
                planLine();
                drawing();
                break;
            case 'B':
                planCharacter();
                drawing();
                break;
            case 'C':
                doPause();
                break;
            case 'D':
                doContinue();
                break;
            case 'E':
                doConcel();
                break;
            default :break;
        }
    }
}



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
   if(ControlMode==0) Serial_Command(); //手柄操作
   else if(ControlMode==1) button_Get();   //按键操作
   else if(ControlMode==2){
     getBottonTask5_Data();
     drawTick();
   }  
   
   if(ControlMode!=2)joyStickPlay();
   Record_ing();  //判断是否记录：开始记录
  
}