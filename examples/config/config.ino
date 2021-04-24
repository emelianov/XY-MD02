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

// RS-485 tranceiver connection pins
#define RX_PIN 18
#define TX_PIN 19
// Direction control pin (-1 if not set)
#define REDE_PIN -1

// First data register
#define XY_BASE 1
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
    Serial.begin(115200); // Set parameters for debug output Serial
    Serial1.begin(9600, SERIAL_8N1, RX_PIN, TX_PIN);  // Set parameters for RS-485 Serial
    mb.begin(&Serial1, REDE_PIN); // Init Modbus object on above mentioned Serial1
    mb.master();  // Set master mode for Modbus object
    Serial.println("Scaning...");
    for (uint8_t i = 1; i < SCAN_DEV; i++) {
        uint16_t tmp = 0xFFFF;
        mb.readIreg(i, XY_BASE, &tmp, 1, cbWrite);  // Start read request of Temperature Ireg
        while (mb.slave())  // Wait for responce or timeout
            mb.task();
        if (ev != Modbus::EX_TIMEOUT) { // If not Timeout
                                        // Timeout is means indeed that no any device with specific id on bus
                                        // If device with specific id is exeist on bus it will respond with the register value of some kind of internal error
            uint16_t val1 = 0xFFFF;
            mb.readIreg(i, XY_BASE + 1, &val1, 1, cbWrite); // Start read of humidity
            while (mb.slave())  // Wait for responce
                mb.task();
            Serial.printf("Got responce for ID = %d. Ireg0 = %d, Ireg1 = %d\n", i, tmp, val1);
        }
    }
    if (CONFIG_DEV) { // If CONFIG_DEV is not 0
        mb.writeHreg(CONFIG_DEV, REG_ID, (uint16_t)NEW_ID); // Send write request to Modbus device <CONFIG_DEV> id to write <NEW_ID> value to Holding register <REG_ID>
        while (mb.slave())  // Wait for responce
            mb.task();
        if (ev == Modbus::EX_SUCCESS) // Check if responce Ok
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