#define PIN 2

#include <Time.h>

volatile int val;
volatile int sec;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PIN), test, RISING);
  val = 0;
  sec = 0;
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println(val);
  delay(1000);
}

void test()
{
  int segundo = second();
  if(segundo > sec)
  {
    val += 1;
    sec = segundo;
  }
}

