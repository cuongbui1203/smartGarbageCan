#include <Servo.h>
#include <SoftwareSerial.h>

#define TX 13
#define RX 12

SoftwareSerial serial2(RX, TX);

#define UP true
#define DOWN false
#define dir1 10
#define pwm1 3
#define dir2 11
#define pwm2 6
#define trigPin 4
#define echoPin 5
#define tiemcan 8
#define servoPin 7
#define ht1 A0
#define ht2 A1
#define emtyled A2
#define processLed A3
#define speed 100


Servo servo;

unsigned long now = 0;
int delayTime = 0;
unsigned long old = 0;
bool run = false;

int timeEp = 0;

int timeOpen = 0;

long duration = 0;
int distance = 0;
bool espReady = false;

void controllMotor1(bool dir, int pwm);
void controllMotor2(bool dir, int pwm);
void caculateDistance();
void controlEp();

void setup() {
  pinMode(dir1, OUTPUT);
  pinMode(pwm1, OUTPUT);
  pinMode(dir2, OUTPUT);
  pinMode(pwm2, OUTPUT);

  pinMode(tiemcan, INPUT_PULLUP);
  pinMode(ht1, INPUT_PULLUP);
  pinMode(ht2, INPUT_PULLUP);

  servo.attach(servoPin);

  serial2.begin(9600);
  Serial.begin(9600);
  while (!espReady) {
    if (Serial.available()) {
      int tg = Serial.read();
      if (char(tg) == 'r') {
        espReady = true;
      }
    }
    serial2.println("e");
  }
}
void loop() {
  // put your main code here, to run repeatedly:
  int tg = millis() - old;
  if (tg >= delayTime) {
    if (!run && !digitalRead(tiemcan)) {
      if (timeOpen == 0) {
        servo.write(50);
        timeOpen++;
      } else {
        timeOpen = 1;
      }
    }

    if (timeOpen > 0) {
      timeOpen++;
    }
    if (timeOpen >= 5) {
      timeOpen = 0;
    }

    caculateDistance();
    run = !run && distance <= 20;
    controlEp();
    old = millis();
  }
}

void controllMotor1(bool dir, int pwm) {
  digitalWrite(dir1, dir);
  analogWrite(pwm1, pwm);
}

void controllMotor2(bool dir, int pwm) {
  digitalWrite(dir2, dir);
  analogWrite(pwm2, pwm);
}

void caculateDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = (duration / 2) / 29.1;

  if (distance >= 400 || distance <= 0) {
    Serial.println("Out of range");
  } else {
    Serial.print(distance);
    Serial.println(" cm");
  }
}

void controlEp() {
  if (run) {
    serial2.print('f');
    if (timeEp == 0) {
      controllMotor1(DOWN, speed);
      controllMotor2(DOWN, speed);
      timeEp++;
    }
    if (timeEp >= 10) {
      controllMotor1(UP, !digitalRead(ht1) ? 0 : speed);
      controllMotor2(UP, !digitalRead(ht2) ? 0 : speed);
      if (analogRead(pwm1) == 0 && analogRead(pwm2) == 0) {
        run = false;
        timeEp = 0;
        serial2.print('e');
      }
    } else {
      timeEp++;
    }
  }
}