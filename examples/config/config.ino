/*
  XY-MD02 configuretion tasks

  (c)2021 Alexander Emelianov (a.m.emelianov@gmail.com)
  https://github.com/emelianov/XY-MD02
*/

#include <ModbusRTU.h>
ModbusRTU mb;

Modbus::ResultCode ev;
bool cbWrite(Modbus::ResultCode event, uint16_t transactionId, void* data) {
    ev = event;
    return true;
}

// Scan devices from 1 to
#define SCAN_DEV 10
// Write configuration to device with id (set 0 to configure none)
#define CONFIG_DEV 0
#define NEW_ID 2
#define NEW_BAUDRATE 0
#define NEW_FIX_T 0
#define NEW_FIX_H 0

// XY-MD02 registers
#define REG_ID 0x0101
#define REG_BAUD 0x0102
#define REG_FIX_T 0x0103
#define REG_FIX_H 0x0104

void setup() {
    Serial.begin(115200);
    Serial1.begin(9600, SERIAL_8N1);
    mb.begin(&Serial1);
    mb.master();
    Serial.println("Scaning...");
    for (uint8_t i = 1; i < SCAN_DEV; i++) {
        uint16_t tmp;
        mb.readIreg(i, 0, &tmp);
        while (mb.slave())
            mb.task();
        if (ev != Modbus::EX_TIMEOUT)
            Serial.printf("Got responce for ID = %d\n", i);
    }
    if (CONFIG_DEV) {
        mb.writeHreg(CONFIG_DEV, REG_ID, (uint16_t)NEW_ID);
        while (mb.slave())
            mb.task();
        if (ev == Modbus::EX_SUCCESS)
            Serial.println("ID changed");
 /* Uncomment if needed to cheange baudrate and measurement corrections
        mb.writeHreg(CONFIG_DEV, REG_BAUD, (uint16_t)NEW_BAUDRATE);
        while (mb.slave())
            mb.task();
        if (ev == Modbus::EX_SUCCESS)
            Serial.println("Baudrate changed");
        mb.writeHreg(CONFIG_DEV, REG_FIX_T, (uint16_t)NEW_FIX_T);
        while (mb.slave())
            mb.task();
        if (ev == Modbus::EX_SUCCESS)
            Serial.println("T correction changed");
        mb.writeHreg(CONFIG_DEV, REG_FIX_H, (uint16_t)NEW_FIX_H);
        while (mb.slave())
            mb.task();
        if (ev == Modbus::EX_SUCCESS)
            Serial.println("H correction changed");
*/
    }
}

void loop() {
    delay(10000);
}