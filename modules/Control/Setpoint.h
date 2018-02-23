#include <Task.h>
extern TaskManager taskManager;
class Setpoint : public Control
{
  public:
    Setpoint(int ch) : Control(1000,ch){
      UpdateFromRom();
    };

    void SetValue(float sp, float w, float d, int s)
    {
      setpoint = sp;
      working = w;
      detecting = d;
      sensor = s;
      SaveToRom();
    }

   
  private:
    float setpoint, working, detecting;
    int sensor;
    virtual bool OnStart()
    {

    }
    virtual void OnUpdate(uint32_t delta_time)
    {
      // Serial.println("Setpoint is running " + String(channel) + " " + String(working) + " " + String(detecting) + " " + String(sensor));
    }

    void UpdateFromRom()
    {
      working = rom_channel[channel-1].working;
      detecting = rom_channel[channel-1].detecting;
      sensor = rom_channel[channel-1].setpoint_sensor;
    }

    void SaveToRom()
    {
      rom_channel[channel-1].working = working;
      rom_channel[channel-1].detecting = detecting;
      rom_channel[channel-1].setpoint_sensor = sensor;
      EEPROM_Manager::Update(channel);
    }

};