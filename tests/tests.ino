#include <ModbusRTU.h>
#include <StreamBuf.h>
#include <XY-MD02.h>

#define BSIZE 1024

uint8_t buf1[BSIZE];
uint8_t buf2[BSIZE];

StreamBuf S1(buf1, BSIZE);
StreamBuf S2(buf2, BSIZE);
DuplexBuf P1(&S1, &S2);
DuplexBuf P2(&S2, &S1);

ModbusRTU slave;
#define SLAVE_ID 1

void setup() {
  Serial.begin(115200);
  slave.begin((Stream*)&P2);
  slave.slave(SLAVE_ID);
  slave.addIreg(0, 500);
  slave.addIreg(1, 600);
  xy_modbus((Stream*)&P1);
  xy_add(SLAVE_ID);
  while (!xy_get(0).lastSuccess) {
    slave.task();
    delay(100);
  }
  Serial.printf("T = %d (expected %d)\n", xy_get(0).T, slave.Ireg(0));
  Serial.printf("H = %d (expected %d)\n", xy_get(0).H, slave.Ireg(1));
}

void loop() {
  xy_md02[0].lastSuccess = 0;
  xy_md02[1].lastSuccess = 0;
  while (!xy_get(0).lastSuccess && !xy_get(1).lastSuccess) {
    slave.task();
    delay(100);
  }
  Serial.printf("T1 = %d (expected %d)\n", xy_get(0).T, slave.Ireg(0));
  Serial.printf("H1 = %d (expected %d)\n", xy_get(0).H, slave.Ireg(1));
  Serial.printf("T1 = %f H1 = %f)\n", (float)xy_get(0).T/10, (float)xy_get(0).H/10);
  slave.Ireg(0, slave.Ireg(0) + 1);
  slave.Ireg(1, slave.Ireg(1) + 1);
  xy_md02[0].lastSuccess = 0;
  xy_md02[1].lastSuccess = 0;
  while (!xy_get(0).lastSuccess && !xy_get(1).lastSuccess) {
    slave.task();
    delay(100);
  }
  Serial.printf("T2 = %d (expected %d)\n", xy_get(1).T, slave.Ireg(0));
  Serial.printf("H2 = %d (expected %d)\n", xy_get(1).H, slave.Ireg(1));
  Serial.printf("T2 = %f H2 = %f)\n", (float)xy_get(1).T/10, (float)xy_get(1).H/10);
  delay(10000);
}