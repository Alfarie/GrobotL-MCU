#include <Task.h>
extern TaskManager taskManager;
#include <LibHumidity.h>

LibHumidity humidity = LibHumidity(0);


// SoftwareSerial sensorCom(5,6);
class Sensor : public Task
{
public:
  static Sensor *s_instance;
  Sensor() : Task(MsToTaskTime(1000))
  {
    sensor.temp = 0;
    sensor.humi = 0;
    sensor.soil = 0;
    sensor.par = 0;
    sensor.vpd = 0;
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

  float GetSensor(int num)
  {
    float value = 0.0f;
    switch (num)
    {
    case 1:
      value = sensor.soil;
      break;
    case 2:
      value = sensor.vpd;
      break;
    case 3:
      value = sensor.par;
      break;
    case 4:
      value = sensor.temp;
      break;
    case 5:
      value = sensor.humi;
      break;
    default:
      value = -1;
    }

    return value;
  }

 

private:
  sensor_s sensor;
  virtual bool OnStart()
  {

  }
  virtual void OnUpdate(uint32_t delta_time)
  {
    sensor.temp = humidity.GetTemperatureC();
    sensor.humi = humidity.GetHumidity();
    sensor.soil = random(49, 51);
    sensor.par = random(1000, 1100);
    sensor.vpd = 1500;
  }
};
Sensor *Sensor::s_instance = 0;