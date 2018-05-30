#include <Task.h>
extern TaskManager taskManager;
#define FT_SENSOR A3
#include "./Solution.h"
#include "./Gy21.h"
#include "./CO2.h"
#include "./Light.h"
class Sensor : public Task
{
public:
  static Sensor *s_instance;
  Sensor() : Task(MsToTaskTime(5))
  {
    sensor.temp = 0;
    sensor.humi = 0;
    sensor.water = 0;
    sensor.ec = 0;
    sensor.ph = 0;
    sensor.co2 = 0;
    sensor.light = 0;


    taskManager.StartTask(Solution::instance());
    taskManager.StartTask(GY21::instance());
    taskManager.StartTask(Co2Sensor::instance());
    taskManager.StartTask(Light::instance());
  };
  static Sensor *instance()
  {
    if (!s_instance)
      s_instance = new Sensor;
    return s_instance;
  }
  sensor_s GetSensors()
  {
    return sensor;
  }

  String GetSensorsString()
  {
    /*String sensorDataJsonStr = "{ \"ec\":" + String(sensor.ec) +
                               ",\"ph\":" + String(sensor.ph) +
                               ",\"water\":" + String(sensor.water) +
                               ",\"light\":" + String(sensor.light) +
                               ",\"temperature\":" + String(sensor.temp) +
                               ",\"humidity\":" + String(sensor.humi) +
                               ",\"co2\":" + String(sensor.co2) +
                               ",\"floating\":" + String(sensor.floating) +
                               ", \"date\":" + "\"" + RTC::instance()->GetDateString() + "\"" +
                               ", \"time\":" + "\"" + RTC::instance()->GetTimeString() + "\"" + "}";
    String data = "{\"type\": \"sensors\",\"data\": " + sensorDataJsonStr + "}";
    */
    
    /*
      {sensors, ec, ph, water_temp, temperature, humidity, co2, light, floating}
    */
    String data =  "{st-sensors," + String(sensor.ec) + 
            "," + String(sensor.ph) + 
            "," + String(sensor.water) + 
            "," + String(sensor.light) + 
            "," + String(sensor.temp) + 
            "," + String(sensor.humi) + 
            "," + String(sensor.co2) + 
            "," + String(sensor.floating) + "}"; 
    return data;
  }
  
  float GetSensor(int num)
  {
    float value = 0.0f;
    switch (num)
    {
    case 0: // 0:vpd, 1:soil, 2:par, 3:temp, 4:humi, 5:co2
      value = sensor.vpd;
      break;
    case 1:
      value = sensor.soil;
      break;
    case 2:
      value = sensor.temp;
      break;
    case 3:
      value = sensor.humi;
      break;
    case 4:
      value = sensor.co2;
      break;
    case 5:
      value = sensor.ec;
      break;
    case 6:
      value = sensor.ph;
      break;
    case 7:
      value = sensor.light;
      break;
    default:
      value = -1;
    }
    return value;
  }

private:
  sensor_s sensor = {0};

  virtual bool OnStart()
  {
    return true;
  }
  
  virtual void OnUpdate(uint32_t delta_time)
  {
    sensor.temp = GY21::instance()->GetTemperature();
    sensor.humi = GY21::instance()->GetHumidity();
    // sensor.vpd = GY21::instance()->GetVpd();
    sensor.ec = Solution::instance()->GetEC() * calibrationData.ecCal;
    sensor.ph = Solution::instance()->GetpH() * calibrationData.phCal;
    sensor.water = Solution::instance()->GetWaterTemp();
    sensor.co2 = Co2Sensor::instance()->GetCO2();
    sensor.light = Light::instance()->GetLight();
    // sensor.floating = digitalRead(FT_SENSOR);
    sensor.floating = Solution::instance()->GetFloating();

    // sensor.temp = 25.0;
    // sensor.humi = 60.0;
    // sensor.water = 24.0;
    // sensor.ec = 1.5;
    // sensor.ph = 6.5;
    // sensor.co2 = 1500;
    // sensor.light = 10000;

  }
};
Sensor *Sensor::s_instance = 0;