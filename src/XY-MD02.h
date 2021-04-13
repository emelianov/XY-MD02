// Arduino/ESP32 XY-MD02 library
// (c)2021, a.m.emelianov@gmail.com

#include <ModbusRTU.h>

// Maximum count of XY-MD02 devices on bus
#define XY_MAX_DEVS 10
// ESP32 Modbus task priority
#define XY_PRIORITY 1
// Default Modbus task sleep time
#define XY_SLEEP 5000
// XY-MD02 First Data Ireg
#define XY_BASE 0
// XY-MD02 Data Ireg Count (should not be changed)
#define XY_COUNT 2

// Thread safe defenitions
#define XY_LOCK xSemaphoreTake(xy_Mutex, portMAX_DELAY);
#define XY_UNLOCK xSemaphoreGive(xy_Mutex);
SemaphoreHandle_t xy_Mutex = NULL;

ModbusRTU XY_MD02;  // ModbusRTU instance
void XY_MD02_task(void* parameter);

// XY-MD02 device data structure
typedef struct xy_parms {
    uint8_t slave_id = 0;   // Set on creation
    uint32_t lastSuccess = 0;// Set each as current millis() time got data from the device
    uint16_t T = 0;         // Temperature
    uint16_t H = 0;         // Humodity
} xy_parms;
// Data structures storage "array" used internally mostly for convenience
class xy_array {
    private:
    xy_parms data[XY_MAX_DEVS]; // XY-MD02 devices records     
    public:      
    xy_parms &operator[](int i) { // Overrride [] to act as array
        if( i > XY_MAX_DEVS )
            return data[0];
        else
            return data[i];
    }
    xy_parms operator[](int i) const {  // Overrride [] to act as array
        if( i > XY_MAX_DEVS )
            return data[0];
        else
            return data[i];
    }
    public:
    xy_parms get(int i) {   // Get XY-MD02 device record at position
                            // Note that object lock by semaphore used
                            // For thread safe operations 
        xy_parms r;
        XY_LOCK
        r = data[i];
        XY_UNLOCK
        return r;
    }
    int16_t add(uint8_t slave) { // Add XY-MD02 device with specific slave_id
                                 // Returns position in the array
        uint8_t i = 0;
        XY_LOCK
        while (data[i].slave_id != 0 && i < XY_MAX_DEVS)
            i++;
        if (i > XY_MAX_DEVS) {
            XY_UNLOCK
            return -1;
        }
        data[i].slave_id = slave;
        XY_UNLOCK
        return i;
    }
};

xy_array xy_md02 = xy_array();  // Predefine array
uint32_t xy_sleep = XY_SLEEP;   // Task sleep variable
 
// Public function to add XY-MD02 device to be queried data from
// Returns the device id
int16_t xy_add(uint8_t slave) {
    int16_t r = xy_md02.add(slave);

    if (r == 0) {               // If first device added we need to create task for quering data
        xTaskCreate(
            XY_MD02_task,       // Task function
            "XY-MD02 Modbus",   // String with name of task
            10000,              // Stack size in bytes
            NULL,               // Parameter passed as input of the task
            XY_PRIORITY,        // Priority of the task
            NULL);              // Task handle
    }

    return r;
}

// Get XY-MD02 device record by id
xy_parms xy_get(uint8_t i) {
    return xy_md02.get(i);
}

// Set Modbus task sleep time
bool xy_read_ms(uint32_t interval = 0) {
    XY_LOCK
    xy_sleep = interval;
    XY_UNLOCK
}

// Initialize Modbus subsystem
bool xy_modbus(HardwareSerial s, int16_t pin = -1) {
    xy_Mutex = xSemaphoreCreateMutex();
    XY_MD02.begin(&s, pin);
    XY_MD02.master();
    return true;
}
bool xy_modbus(Stream* s) { // The same for tests
    xy_Mutex = xSemaphoreCreateMutex();
    XY_MD02.begin(s);
    XY_MD02.master();
    return true;
}

// Modbus query callback to process the result
Modbus::ResultCode xy_err;
bool xy_cb(Modbus::ResultCode event, uint16_t, void*) {
    xy_err = event;
    return true;
}
// Modbus task
void XY_MD02_task(void* parameter) {
    struct {    // Temprary structure for incoming data
        uint16_t T;
        uint16_t H;
    } values;
    uint8_t i = 0;  // Current XY-MD02 device id
    while (true) {  // Do forever
        XY_LOCK
        bool active = xy_md02[0].slave_id > 0;
        XY_UNLOCK
        if (!active) {  // If no devices just sleep. Normally should not happens.
            delay(XY_SLEEP);
            continue;
        }
        // Send request for reading Iregs
        XY_MD02.readIreg(xy_md02[i].slave_id, XY_BASE, (uint16_t*)&values, XY_COUNT, xy_cb);
        while (XY_MD02.slave()) {   // Wait for responce or timeout
            delay(100);
            XY_MD02.task();
        }
        // At this point xy_err filler by operation result bay xu_cb() callback
        if (xy_err == Modbus::EX_SUCCESS) { // If result is success fill the data
            XY_LOCK
            xy_md02[i].T = values.T;
            xy_md02[i].H = values.H;
            xy_md02[i].lastSuccess = millis();
            XY_UNLOCK
        }
        i++;
        if (xy_md02[i].slave_id == 0)   // If next element is empty 
            i = 0;                      // go to first one
        delay(xy_sleep);                // Sleep
    }
    vTaskDelete(NULL);  // Kill task if exit loop
}