#include <Task.h>
extern TaskManager taskManager;
class Communication : public Task
{
public:
  static Communication *s_instance;
  Communication() : Task(MsToTaskTime(5))
  {
    cmdStr = "";
    cmdNumber = 0;
    cmdSize = 0;
    size = 0;
    Serial.begin(115200);
  };
  static Communication *instance()
  {
    if (!s_instance)
      s_instance = new Communication;
    return s_instance;
  }

private:
  String cmdStr;
  char res[100];
  int size, cmdNumber, cmdSize;
  virtual bool OnStart()
  {
  }
  virtual void OnUpdate(uint32_t delta_time)
  {
    //{timer,1,1,20-60,90-150,200-260}{setpoint,2,50.5,30,30,1}{manual,3,0}{manual,4,1}

    while (Serial.available())
    {
      if (Serial.read() == '{')
      {
        int size = 0;
        while (true)
        {
          if (Serial.available())
          {
            char ch = Serial.read();
            if (ch == '}')
            {
              res[size] = '\0';
              String strRes = res;
              // CommandProcess(String(res));
              cmdStr += "{" + strRes + "}";
              cmdNumber++;
              break;
            }
            else
            {
              res[size] = ch;
              size++;
            }
          }
        }
      }
    }

    if (cmdNumber > 0)
    {

      for (int i = 0; i < cmdNumber; i++)
      {
        String subcom = cmdStr.substring(1, cmdStr.indexOf("}"));
        CommandProcess(subcom);
        cmdStr = cmdStr.substring(cmdStr.indexOf("}") + 1);
      }

      cmdNumber = 0;
      cmdStr = "";
    }
  }
  void ExtractDataFloat(float *data, int size, String res)
  {
    int i = 0, si = 0, ei, j = 0;
    while (j < size)
    {
      int index = res.indexOf(",");
      String a = res.substring(0, index);
      data[j] = a.toFloat();

      si = index;
      res = res.substring(index + 1);
      j++;
    }
  }

  void ExtractDataInt(int *data, int size, String res)
  {
    int i = 0, si = 0, ei, j = 0;
    while (j < size)
    {
      int index = res.indexOf(",");
      String a = res.substring(0, index);
      data[j] = a.toInt();

      si = index;
      res = res.substring(index + 1);
      j++;
    }
  }

  void ExtractDataString(String *data, int &size, String res)
  {
    int i = 0, si = 0, ei, j = 0;
    while (j < 20)
    {
      int index = res.indexOf(",");

      String a = res.substring(0, index);
      data[j] = a;
      // Serial.println(data[j]);
      si = index;
      res = res.substring(index + 1);
      j++;
      if (index == -1)
      {
        size = j;
        return;
      }
    }
  }

  timer_s *TimerStringToTimer(String *timer, timer_s *vtimer, int size)
  {
    for (int i = 0; i < size; i++)
    {
      int index = timer[i].indexOf("-");
      String startStr = timer[i].substring(0, index);
      String endStr = timer[i].substring(index + 1);

      int start = startStr.toInt();
      int end = endStr.toInt();
      timer_s t = {start, end};
      vtimer[i] = t;
    }
  }
  //{timer,1,1,20-60,90-150,200-260}{setpoint,2,50.5,30,30,1}{manual,3,0}{manual,4,1}
  bool CommandProcess(String res)
  {
    // Serial.println(res);
    // {datetime,8,1,17,6,31}
    if (res.startsWith("sensor"))
    {
      sensor_s sensor = Sensor::instance()->GetSensors();
      // Serial.println("Sensor from mcu");
      String sensorDataJsonStr = "{ \"soil\":" + String(sensor.soil) +
                                 ",\"vpd\":" + String(sensor.vpd) +
                                 ",\"temp\":" + String(sensor.temp) +
                                 ",\"humi\":" + String(sensor.humi) +
                                 ",\"actuator\":" + ChannelHanler::GetChannelStatus() +
                                 ", \"date\":" + "\"" + DateTime::instance()->GetDateString() + "\"" +
                                 ", \"time\":" + "\"" + DateTime::instance()->GetTimeString() + "\"" + "}";
      String data = "{\"type\": \"sensor\",\"data\": " + sensorDataJsonStr + "}";
      Serial.println(data);
    }
    if (res.startsWith("setting"))
    {
      Serial.println(ChannelHanler::instance()->JsonTimer());
    }

    if (res.startsWith("datetime"))
    {
      res.replace("datetime,", "");
      res.trim();
      int dt[5] = {0, 0, 0, 0, 0};
      ExtractDataInt(dt, 5, res);
      // (byte s, byte m, byte h, byte dow, byte dom, byte mo, byte y)
      DateTime::instance()->setDateDs1307(0, dt[4], dt[3], 0, dt[0], dt[1], dt[2]);
    }

    else if(res.startsWith("mode"))
    {
      res.replace("mode,", "");
      res.trim();
      int mode[2] = {0, 0};
      ExtractDataInt(mode, 2, res);
      int ch = (int)mode[0];
      rom_channel[ch - 1].channel_mode = mode[1];
      EEPROM_Manager::Update(ch);
      ChannelHanler::instance()->Update(ch);
    }
    
    //{manual, channel, status}
    // {manual, 1,1}
    else if (res.startsWith("manual"))
    {
      res.replace("manual,", "");
      res.trim();
      int mode[2] = {0, 0};
      ExtractDataInt(mode, 2, res);
      int ch = (int)mode[0];
      rom_channel[ch - 1].channel_mode = 0;
      rom_channel[ch - 1].manual_status = mode[1];
      EEPROM_Manager::Update(ch);
      ChannelHanler::instance()->Update(ch);
    }
    // {setpoint,channel,setpoint_value, working, detecting, sensor}
    // {setpoint,1,50.5,30,30,1}
    else if (res.startsWith("setpoint"))
    {
      res.replace("setpoint,", "");
      res.trim();
      float mode[5];
      ExtractDataFloat(mode, 5, res);
      int ch = (int)mode[0];
      rom_channel[ch - 1].channel_mode = 2;
      rom_channel[ch - 1].setpoint = mode[1];
      rom_channel[ch - 1].working = mode[2];
      rom_channel[ch - 1].detecting = mode[3];
      rom_channel[ch - 1].setpoint_sensor = (byte)mode[4];
      EEPROM_Manager::Update(ch);
      ChannelHanler::instance()->Update(ch);
    }
    //{timer,1,1,20-60,90-150,200-260}
    else if (res.startsWith("timer"))
    {
      res.replace("timer,", "");
      res.trim();
      int ch = res.substring(0, 1).toInt();
      int timer_mode = res.substring(2, 3).toInt();
      res = res.substring(4);
      String timer[15];
      int size = 0;
      ExtractDataString(timer, size, res);
      timer_s timerlist[8];
      TimerStringToTimer(timer, timerlist, size);
      
      memcpy(rom_channel[ch - 1].timer_list, timerlist, 8 * sizeof(timer_s));
      
      rom_channel[ch - 1].channel_mode = 1;
      rom_channel[ch - 1].timer_size = size;
      rom_channel[ch - 1].timer_mode = timer_mode;
      EEPROM_Manager::Update(ch);
      ChannelHanler::instance()->Update(ch);
    }
    //{setbound, channel, upper,lower,sensor}
    // {setbound, 1, 40.0, 50.0, 1}
    else if (res.startsWith("setbound"))
    {
      res.replace("setbound,", "");
      res.trim();
      float mode[4];
      ExtractDataFloat(mode, 4, res);
      int ch = (int)mode[0];
      rom_channel[ch - 1].channel_mode = 3;
      rom_channel[ch - 1].setbound_upper = mode[1];
      rom_channel[ch - 1].setbound_lower = mode[2];
      rom_channel[ch - 1].setpoint_sensor = (byte)mode[3];
      EEPROM_Manager::Update(ch);
      ChannelHanler::instance()->Update(ch);
    }

    else if (res.startsWith("irrigation"))
    {
      res.replace("irrigation,", "");
      res.trim();
      float mode[5];
      ExtractDataFloat(mode, 5, res);
      int ch = (int)mode[0];
      rom_channel[ch - 1].channel_mode = 4;
      rom_channel[ch - 1].irr_mode = (int)mode[1];
      rom_channel[ch - 1].soil_upper = mode[2];
      rom_channel[ch - 1].soil_lower = mode[3];
      rom_channel[ch - 1].par_accum = mode[4];
      EEPROM_Manager::Update(ch);
      ChannelHanler::instance()->Update(ch);
    }
  }
};
Communication *Communication::s_instance = 0;

//  while (Serial.available())
//     {
//       if (Serial.read() == '{')
//       {
//         size = 0;
//         while (true)
//         {
//           if (Serial.available())
//           {
//             char ch = Serial.read();
//             if (ch == '}')
//             {
//               res[size] = '\0';
//               String strRes = res;
//               // CommandProcess(String(res));
//               cmdStr += "{" + strRes + "}";
//               cmdSize++;
//               break;
//             }
//             else{
//               res[size]= ch;
//               size++;
//             }
//           }
//         }

//       }
//     }