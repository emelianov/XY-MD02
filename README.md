# XY-MD02 Modbus tools for ESP32

As ModbusRTU data exchange is slow process (especially in case if device not responding) main idea is to use didicated background task that periodically executes data read requsts from XY-MD02 devices. Main task have ability quickly get last values and time when they was received.

## Prerequirements

https://github.com/emelianov/modbus-esp8266
# API

```c
bool xy_modbus(HardwareSerial s, int16_t pin = -1);
```
Initialize the library. Should be called prior other library calls.
- `s` Serial port to use for Modbus RTU communications
- `pin` Read/Write control pin
*Returns:* `true` on success

```c
bool xy_read_ms(uint32_t interval = 0);
```
Set delay between data read sessions.
- `interval` Delay between devices reads
*Returns:* `true` on success

```c
xy_parms xy_get(uint8_t i);
```
Returns last avaiolable data fot the specific device.
- `i` Posision of device to get data
*Returns:* `xy_parms` structure of filled by the device data.

```c
int16_t xy_add(uint8_t slave);
```
Adds the device.
`slave` Slave ID of the device to add
*Returns:* Device position that should be used for `xy_get()` or `-1` on error.

```c
typedef struct xy_parms {
    uint8_t slave_id = 0;   // Set on creation
    uint32_t lastSuccess = 0;// Set each as current millis() time got data from the device
    uint16_t T = 0;         // Temperature
    uint16_t H = 0;         // Humodity
} xy_parms;
```

(c)2021 Alexander Emelianov (a.m.emelianov@gmail.com)
https://github.com/emelianov/XY-MD02