#include <Task.h>
extern TaskManager taskManager;
class Irrigation : public Control
{
  public:
    Irrigation(int ch) : Control(1000, ch)
    {
        UpdateFromRom();
    };

    void SetValue(float su, float sl, float pac, int mode)
    {

        SaveToRom();
    }
  private:
    float soil_upper, soil_lower,par_accum;
    int mode;
    virtual bool OnStart()
    {

    }
    virtual void OnUpdate(uint32_t delta_time)
    {
    
    }

    void UpdateFromRom()
    {
        soil_upper = rom_channel[channel-1].soil_upper;
        soil_lower = rom_channel[channel-1].soil_lower;
        par_accum = rom_channel[channel-1].par_accum;
    }
    void SaveToRom()
    {
        rom_channel[channel-1].soil_upper = soil_upper;
        rom_channel[channel-1].soil_lower = soil_lower;
        rom_channel[channel-1].par_accum =  par_accum;
        EEPROM_Manager::Update(channel);
    }
};