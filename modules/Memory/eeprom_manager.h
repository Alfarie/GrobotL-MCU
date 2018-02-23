#include <EEPROM.h>
#include "./address.h"

struct data_table_s
{
    byte channel_mode;
    byte manual_status;

    byte timer_mode;
    byte timer_size;
    timer_s timer_list[8];

    float setpoint;
    float working;
    float detecting;
    byte setpoint_sensor;

    float setbound_upper;
    float setbound_lower;
    byte setbound_sensor;

    // irrigation
    float soil_upper;
    float soil_lower;
    float par_accum;
    byte irr_mode;
};
data_table_s rom_channel[4];

class EEPROM_Manager
{
  public:
    EEPROM_Manager(){};
    static int init_byte;
    static void InitEEPROM()
    {
        int init;
        EEPROM.get(init_byte, init);
        Serial.println(init_byte);
        if (init == 2)
        {
            DisplayLog::PrintInfo("Initialize eeprom GetAll data");
            GetAll();
        }
        else
        {
            DisplayLog::PrintInfo("Initialize eeprom create new setting");
            data_table_s dts1, dts2, dts3, dts4;
            dts1 = {0};
            dts2 = {0};
            dts3 = {0};
            dts4 = {0};

            dts1.channel_mode = 0; 
            dts2.channel_mode = 1;
            dts3.channel_mode = 2;
            dts4.channel_mode = 3;
            EEPROM.put(100, dts1);
            EEPROM.put(200, dts2);
            EEPROM.put(300, dts3);
            EEPROM.put(400, dts4);
            EEPROM.put(init_byte, 2);
            GetAll();
        }
        
    }
    static void Update(int channel)
    {
        DisplayLog::PrintInfo(" Update to Channel " + String(channel) + " " + String(channel_list[channel - 1]));

        // Serial.println("size: " + String(rom_channel[channel - 1].timer_size));
        // for(int i = 0 ; i < rom_channel[channel - 1].timer_size; i++){
        //     Serial.print( String(rom_channel[channel - 1].timer_list[i].st) + "-" + String(rom_channel[channel - 1].timer_list[i].en) + " ");
        // }
        // Serial.println();
        EEPROM.put(channel_list[channel - 1], rom_channel[channel - 1]);
    }

    static void GetAll()
    {
        EEPROM.get(channel_list[0], rom_channel[0]);
        EEPROM.get(channel_list[1], rom_channel[1]);
        EEPROM.get(channel_list[2], rom_channel[2]);
        EEPROM.get(channel_list[3], rom_channel[3]);
    }

  private:
    static int channel_list[4];
};

int EEPROM_Manager::init_byte = EEPROM.length() - 1;
int EEPROM_Manager::channel_list[4] = {100, 200, 300, 400};