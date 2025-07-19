#include <Encoder.h>
#include <Arduino.h>

// Motor driver pins
#define AIN1 14
#define AIN2 27
#define PWMA 26
#define BIN1 25
#define BIN2 33
#define PWMB 32
#define STBY 12

Encoder enc1(34,35), enc2(36,39);

const int freq = 20000, resolution = 8;
const int chA = 0, chB = 1;

float Kp = 2.0, Ki = 5.0, Kd = 0.2;
const float dt = 0.1;

float prev_e1 =0, prev_e2=0, int1=0, int2=0;

void setup(){
  Serial.begin(115200);
  pinMode(AIN1,OUTPUT); pinMode(AIN2,OUTPUT);
  pinMode(BIN1,OUTPUT); pinMode(BIN2,OUTPUT);
  pinMode(STBY,OUTPUT); digitalWrite(STBY,HIGH);
  ledcSetup(chA,freq,resolution); ledcAttachPin(PWMA,chA);
  ledcSetup(chB,freq,resolution); ledcAttachPin(PWMB,chB);
}

void setMotor(int m,int pwm,bool dir){
  bool in1 = m==1? AIN1: BIN1;
  bool in2 = m==1? AIN2: BIN2;
  int ch = m==1? chA:chB;
  digitalWrite(in1,dir); digitalWrite(in2,!dir);
  ledcWrite(ch,pwm);
}

void loop(){
  static unsigned long ts=0;
  if(millis()-ts < dt*1000) return;
  ts = millis();

  float target=200;
  float s1 = enc1.read()/dt, s2 = enc2.read()/dt;
  enc1.write(0); enc2.write(0);

  float e1 = target - s1, e2=target-s2;
  int1 += e1*dt; int2+=e2*dt;
  float d1 = (e1 - prev_e1)/dt, d2 = (e2 - prev_e2)/dt;

  float u1 = Kp*e1 + Ki*int1 + Kd*d1;
  float u2 = Kp*e2 + Ki*int2 + Kd*d2;

  prev_e1=e1; prev_e2=e2;

  int p1 = constrain(int(abs(u1)), 0, 255);
  int p2 = constrain(int(abs(u2)), 0, 255);
  setMotor(1, p1, u1>0);
  setMotor(2, p2, u2>0);

  Serial.printf("T:%.0f | S1:%.0f E1:%.0f PWM1:%d | S2:%.0f PWM2:%d\n",
                target, s1, e1, p1, s2, p2);
}
