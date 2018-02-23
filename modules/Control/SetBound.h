#include <Task.h>
extern TaskManager taskManager;
class SetBound : public Control
{
  public:
    SetBound(int  ch) : Control(1000, ch){
      UpdateFromRom();
    }

    void SetValue(float u, float l, int s){
      upper = u;
      lower = l;
      sensor = s;

      SaveToRom();
    }

   

  private:
    float upper, lower;
    int sensor;
    virtual bool OnStart()
    {

    }
    virtual void OnUpdate(uint32_t delta_time)
    {
      // Serial.println("Setbound is running " + String(channel) + " " + String(lower) +"/"+String(upper) + " " + String(sensor));
    }

    void UpdateFromRom()
    {
      upper = rom_channel[channel-1].setbound_upper;
      lower = rom_channel[channel-1].setbound_lower;
      sensor = rom_channel[channel-1].setbound_sensor;
    }

    void SaveToRom()
    {
      rom_channel[channel-1].setbound_upper = upper;
      rom_channel[channel-1].setbound_lower = lower;
      rom_channel[channel-1].setbound_sensor = sensor;
      EEPROM_Manager::Update(channel);
    }
};