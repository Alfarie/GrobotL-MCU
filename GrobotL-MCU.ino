#include <Task.h>
TaskManager taskManager;
#include "./modules/Helper/DisplayLog.h"

#include <Wire.h>
#include <EEPROM.h>

#define fanPin 4
#define pumpPin 3
#define ledPin 2

int ON = HIGH;
int OFF = LOW;

struct timer_s
{
    int st;
    int en; // end or working depend on timer mode
};

struct sensor_s
{
    float soil;
    float vpd;
    float par;
    float temp;
    float humi;
};

#include "./modules/Control/Control.h"
Control *channel[4];
int ChannelGpio[4] = {fanPin,pumpPin,ledPin,5};
int ChannelStatus[4] = {LOW,LOW,LOW,LOW};
void DigitalWrite(int ch, int status){
    digitalWrite(ChannelGpio[ch], status);
    ChannelStatus[ch] = status;
}

//General module
#include "./modules/Memory/eeprom_manager.h"
#include "./modules/DateTime.h"
#include "./modules/Sensors/Sensor.h"

#include "./modules/Control/Manual.h"
#include "./modules/Control/Timer.h"
#include "./modules/Control/Setpoint.h"
#include "./modules/Control/SetBound.h"
#include "./modules/Control/Irrigation.h"

#include "./modules/Control/ControlFactory.h"
#include "./modules/ChannelHandler.h"
#include "./modules/Communication.h"

void setup()
{
    Wire.begin();
    Serial.begin(115200);
    EEPROM_Manager::InitEEPROM();
    taskManager.StartTask(DateTime::instance());
    taskManager.StartTask(Sensor::instance());
    taskManager.StartTask(Communication::instance());
    
    ChannelHanler::instance();

}
void loop()
{
    taskManager.Loop();
}
