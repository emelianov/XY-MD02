# XY-MD02 Modbus tools for ESP32

As ModbusRTU data exchange is slow process (especially in case if device not responding) main idea is to use didicated background task that periodically executes data read requsts from XY-MD02 devices. Main task have ability quickly get last values and time when they was received.

# API

```c
bool xy_modbus(HardwareSerial s, int16_t pin = -1);
```

```c
bool xy_read_ms(uint32_t interval = 0);
```
```c
xy_parms xy_get(uint8_t i);
```

```c
int16_t xy_add(uint8_t slave);
```

```c
typedef struct xy_parms {
    uint8_t slave_id = 0;   // Set on creation
    uint32_t lastSuccess = 0;// Set each as current millis() time got data from the device
    uint16_t T = 0;         // Temperature
    uint16_t H = 0;         // Humodity
} xy_parms;
```