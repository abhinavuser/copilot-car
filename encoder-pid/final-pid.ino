#include <Encoder.h>
#include <Arduino.h>
#define AIN1 14
#define AIN2 27
#define PWMA 26
#define BIN1 25
#define BIN2 33
#define PWMB 32
#define STBY 12

// === Encoder Inputs ===
Encoder enc1(34, 35); // Motor 1
Encoder enc2(36, 39); // Motor 2

// === PWM Channels ===
const int freq = 20000;
const int resolution = 8;
const int channelA = 0;
const int channelB = 1;

// === PID Tuning Parameters (final) ===
float Kp = 2.2;
float Ki = 8.0;
float Kd = 0.1;

float dt = 0.1; // 100 ms control loop time
float integral1 = 0, integral2 = 0;
float prev_error1 = 0, prev_error2 = 0;

void setup() {
  Serial.begin(115200);
  pinMode(AIN1, OUTPUT); pinMode(AIN2, OUTPUT);
  pinMode(BIN1, OUTPUT); pinMode(BIN2, OUTPUT);
  pinMode(STBY, OUTPUT); digitalWrite(STBY, HIGH);
  ledcSetup(channelA, freq, resolution);
  ledcAttachPin(PWMA, channelA);
  ledcSetup(channelB, freq, resolution);
  ledcAttachPin(PWMB, channelB);
}

void setMotor(int motor, int pwm, bool direction) {
  int in1 = (motor == 1) ? AIN1 : BIN1;
  int in2 = (motor == 1) ? AIN2 : BIN2;
  int channel = (motor == 1) ? channelA : channelB;

  digitalWrite(in1, direction);
  digitalWrite(in2, !direction);
  ledcWrite(channel, pwm);
}

void loop() {
  static unsigned long lastControl = 0;
  if (millis() - lastControl < dt * 1000) return;
  lastControl = millis();

  // === Target speed in ticks/sec ===
  float target1 = 2000;
  float target2 = 2000;

  float speed1 = enc1.read() / dt;
  float speed2 = enc2.read() / dt;
  enc1.write(0);
  enc2.write(0);

  // === Motor 1 PID ===
  float error1 = target1 - speed1;
  integral1 += error1 * dt;
  float derivative1 = (error1 - prev_error1) / dt;
  float output1 = Kp * error1 + Ki * integral1 + Kd * derivative1;
  prev_error1 = error1;

  // === Motor 2 PID ===
  float error2 = target2 - speed2;
  integral2 += error2 * dt;
  float derivative2 = (error2 - prev_error2) / dt;
  float output2 = Kp * error2 + Ki * integral2 + Kd * derivative2;
  prev_error2 = error2;

  // === Apply to motors ===
  int pwm1 = constrain(int(abs(output1)), 0, 255);
  int pwm2 = constrain(int(abs(output2)), 0, 255);
  bool dir1 = output1 > 0;
  bool dir2 = output2 > 0;

  setMotor(1, pwm1, dir1);
  setMotor(2, pwm2, dir2);

  Serial.printf("Target: %.0f | M1: %.0f PWM1: %d | M2: %.0f PWM2: %d\n",
                target1, speed1, pwm1, speed2, pwm2);
}
