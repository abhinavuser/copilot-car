#include <Encoder.h>
#include <Arduino.h>

// Motor driver pins...
// [Pins omitted for brevity; same as above]

Encoder enc1(34,35), enc2(36,39);

// PID parameters (tweak based on motor behavior)
float Kp = 1.0, Ki = 0.5, Kd = 0.05;
float dt = 0.1;  // 100ms control loop time

long prev_error1=0, prev_error2=0;
float integral1=0, integral2=0;

void setup() {
  Serial.begin(115200);
  // pinMode and motor initialization here...

  pinMode(STBY, OUTPUT);
  digitalWrite(STBY, HIGH);
}

void setMotor(int motor, int pwm, bool dir) {
  int in1 = (motor==1 ? AIN1 : BIN1);
  int in2 = (motor==1 ? AIN2 : BIN2);
  int pwmpin = (motor==1 ? PWMA : PWMB);
  digitalWrite(in1, dir);
  digitalWrite(in2, !dir);
  ledcWrite(channels[motor-1], pwm);
}

void loop() {
  static unsigned long lastTime = 0;
  if (millis() - lastTime < dt*1000) return;
  lastTime = millis();

  // target speeds in ticks/second
  float target1 = 200, target2 = 200;
  float speed1 = (enc1.read() / dt);
  float speed2 = (enc2.read() / dt);
  enc1.write(0); enc2.write(0);

  // PID error
  float err1 = target1 - speed1;
  integral1 += err1 * dt;
  float derivative1 = (err1 - prev_error1) / dt;
  float out1 = Kp*err1 + Ki*integral1 + Kd*derivative1;
  prev_error1 = err1;

  float err2 = target2 - speed2;
  integral2 += err2 * dt;
  float derivative2 = (err2 - prev_error2) / dt;
  float out2 = Kp*err2 + Ki*integral2 + Kd*derivative2;
  prev_error2 = err2;

  int pwm1 = constrain(int(abs(out1)), 0, 255);
  int pwm2 = constrain(int(abs(out2)), 0, 255);
  bool dir1 = out1 > 0, dir2 = out2 > 0;

  setMotor(1, pwm1, dir1);
  setMotor(2, pwm2, dir2);

  Serial.printf("T1:%.0f S1:%.0f PWM1:%d | T2:%.0f S2:%.0f PWM2:%d\n", 
                target1, speed1, pwm1, target2, speed2, pwm2);
}