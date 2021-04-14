/*
  XY-MD02 read example

  (c)2021 Alexander Emelianov (a.m.emelianov@gmail.com)
  https://github.com/emelianov/XY-MD02
*/

#include <XY-MD02.h>

#define SLAVE_ID1 1
#defien SLAVE_ID2 2

uint32_t lastDisplay = 0;
int16_t d1 = -1;
int16_t d2 = -1;

void setup() {
  Serial.begin(115200);
  Serial1.begin(9600);
  xy_modbus(Serial1);
  d1 = xy_add(SLAVE_ID1);
  d2 = xy_add(SLAVE_ID2);
}

void loop() {
  if (xy_get(d1).lastSuccess > lastDisplay || xy_get(d2).lastSuccess > lastDisplay) {
    Serial.printf("T1 = %f H1 = %f)\n", (float)xy_get(d1).T/10, (float)xy_get(d1).H/10);
    Serial.printf("T1 = %f H1 = %f)\n", (float)xy_get(d1).T/10, (float)xy_get(d1).H/10);
    lastDisplay = millis();
  }
  delay(1000);
}