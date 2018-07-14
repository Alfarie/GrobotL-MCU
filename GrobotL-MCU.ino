#include <Task.h>
TaskManager taskManager;

#include <Wire.h>
#include <EEPROM.h>
#define EEPROM_SIZE 1024
#define CHANNEL_NUMBER 6
#define VERSION 2.0
#define PROJECT "GROBOT-L"
#define UPLOADDATE String(__DATE__) + " " +  String(__TIME__)

#define ledPin 2
#define pumpPin 4
#define fanPin 6
#define ch4Pin 7
#define ch5Pin 3
#define ch6Pin 5

String ShowBoardInfo(){
    String str = "INFOBOARD-VERSION" + String(VERSION) + "\r\n";
    str += "INFOPROJECT-NAME "+ String(PROJECT) + "\r\n";
    str += "INFODATE-" + String(UPLOADDATE) + "\r\n";
    return str;
}

HardwareSerial &mpuCom    = Serial;
HardwareSerial &debugCom  = Serial2;
HardwareSerial &testCom = Serial3;

#include "./modules/Helper/DisplayLog.h"

int CH_ON = HIGH;
int CH_OFF = LOW;
String MCU_STATE = "NRDY";

struct timer_s
{
    int st;
    int en; // end or working depend CH_ON timer mode
};

struct sensor_s
{
    float soil;
    float vpd;
    float ec;
    float ph;
    float water;
    float temp;
    float humi;
    float co2;
    float light;
    boolean floating;
};

#include "./modules/Control/Control.h"
Control *channel[CHANNEL_NUMBER];

int ChannelGpio[CHANNEL_NUMBER] = {ledPin,pumpPin,fanPin,ch4Pin,ch5Pin,ch6Pin};
int ChannelStatus[CHANNEL_NUMBER] = {LOW,LOW,LOW,LOW,LOW,LOW};

void DigitalWrite(int ch, int status){
    digitalWrite(ChannelGpio[ch], status);
    ChannelStatus[ch] = status;
}

//General module
#include "./modules/Memory/eeprom_manager.h"
// #include "./modules/DateTime.h"
#include "./modules/RTC.h"
#include "./modules/Sensors/Sensor.h"

//water process
#include "./modules/Process/WaterProcess.h"
WaterProcess *wt_process[6];
#include "./modules/Process/WaterProcessControl.h"

#include "./modules/Control/Manual.h"
#include "./modules/Control/Timer.h"
#include "./modules/Control/Setpoint.h"
#include "./modules/Control/SetBound.h"
#include "./modules/Control/Irrigation.h"

#include "./modules/Control/ControlFactory.h"
#include "./modules/ChannelHandler.h"
#include "./modules/Memory/MemoryCheck.h"

#include "./modules/Communication.h"

#include "./modules/Helper/Puppet.h"

void setup()
{
    // Puppet::instance();
    Wire.begin();

    debugCom.begin(115200);
    mpuCom.begin(115200);
    testCom.begin(115200);

    debugCom.println("Initializing...");
    debugCom.println(ShowBoardInfo());
    mpuCom.println(ShowBoardInfo());
    //testCom.println(ShowBoardInfo());

    EEPROM_Manager::InitEEPROM();
    taskManager.StartTask(RTC::instance());
    taskManager.StartTask(Sensor::instance());
    taskManager.StartTask(Communication::instance());
    ChannelHanler::instance();
    // taskManager.StartTask(WaterProcessControl::instance());
    MCU_STATE = "RDY";
    mpuCom.println(MCU_STATE);
}
void loop()
{
    taskManager.Loop();
}


