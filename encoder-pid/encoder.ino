#include <Encoder.h>
#include <Arduino.h>

// === Motor Driver Pins (Adjust as needed) ===
#define AIN1 14
#define AIN2 27
#define PWMA 26

#define BIN1 25
#define BIN2 33
#define PWMB 32

#define STBY 12

// === Encoder Pins ===
Encoder enc1(34, 35);
Encoder enc2(36, 39);

// === PWM Setup ===
const int freq = 20000;
const int resolution = 8;
const int channelA = 0;
const int channelB = 1;

// === PID Parameters (Ziegler–Nichols Tuned) ===
float Kp = 1.5;
float Ki = 3.75;
float Kd = 0.15;

float dt = 0.1; // 100 ms loop
long prev_error1 = 0, prev_error2 = 0;
float integral1 = 0, integral2 = 0;

void setup() {
  Serial.begin(115200);

  pinMode(AIN1, OUTPUT); pinMode(AIN2, OUTPUT);
  pinMode(BIN1, OUTPUT); pinMode(BIN2, OUTPUT);
  pinMode(STBY, OUTPUT);
  digitalWrite(STBY, HIGH);

  // PWM setup
  ledcSetup(channelA, freq, resolution);
  ledcAttachPin(PWMA, channelA);

  ledcSetup(channelB, freq, resolution);
  ledcAttachPin(PWMB, channelB);
}

void setMotor(int motor, int pwm, bool dir) {
  int in1 = (motor == 1) ? AIN1 : BIN1;
  int in2 = (motor == 1) ? AIN2 : BIN2;
  int pwmPin = (motor == 1) ? PWMA : PWMB;
  int channel = (motor == 1) ? channelA : channelB;

  digitalWrite(in1, dir);
  digitalWrite(in2, !dir);
  ledcWrite(channel, pwm);
}

void loop() {
  static unsigned long lastTime = 0;
  if (millis() - lastTime < dt * 1000) return;
  lastTime = millis();

  // === Set Target Speeds ===
  float target1 = 200; // ticks per second
  float target2 = 200;

  // === Measure Speeds ===
  float speed1 = enc1.read() / dt;
  float speed2 = enc2.read() / dt;
  enc1.write(0);
  enc2.write(0);

  // === PID Motor 1 ===
  float error1 = target1 - speed1;
  integral1 += error1 * dt;
  float derivative1 = (error1 - prev_error1) / dt;
  float output1 = Kp * error1 + Ki * integral1 + Kd * derivative1;
  prev_error1 = error1;

  // === PID Motor 2 ===
  float error2 = target2 - speed2;
  integral2 += error2 * dt;
  float derivative2 = (error2 - prev_error2) / dt;
  float output2 = Kp * error2 + Ki * integral2 + Kd * derivative2;
  prev_error2 = error2;

  // === Set PWM & Direction ===
  int pwm1 = constrain(abs((int)output1), 0, 255);
  int pwm2 = constrain(abs((int)output2), 0, 255);
  bool dir1 = output1 > 0;
  bool dir2 = output2 > 0;

  setMotor(1, pwm1, dir1);
  setMotor(2, pwm2, dir2);

  Serial.printf("T1:%.0f S1:%.0f PWM1:%d | T2:%.0f S2:%.0f PWM2:%d\n",
                target1, speed1, pwm1, target2, speed2, pwm2);
}
