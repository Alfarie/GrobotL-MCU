#include <Task.h>
extern TaskManager taskManager;
class ChannelHanler : public Task
{
  public:
    static ChannelHanler *s_instance;
    
    ChannelHanler() : Task(MsToTaskTime(100))
    {
        for (int i = 0; i < 4; i++)
        {
            pinMode(ChannelGpio[i], OUTPUT);
        }
        Init();
    };
    static ChannelHanler *instance()
    {
        if (!s_instance)
            s_instance = new ChannelHanler;
        return s_instance;
    }
    
    void Update(int ch)
    {
        taskManager.StopTask(channel[ch - 1]);
        int mode = rom_channel[ch - 1].channel_mode;
        switch (mode)
        {
        case 0:
            channel[ch - 1] = ControlFactory::instance("manual", ch);
            taskManager.StartTask(channel[ch - 1]);
            break;
        case 1:
            channel[ch - 1] = ControlFactory::instance("timer", ch);
            taskManager.StartTask(channel[ch - 1]);
            break;
        case 2:
            channel[ch - 1] = ControlFactory::instance("setpoint", ch);
            taskManager.StartTask(channel[ch - 1]);
            break;
        case 3:
            channel[ch - 1] = ControlFactory::instance("setbound", ch);
            taskManager.StartTask(channel[ch - 1]);
            break;
        case 4:
            channel[ch - 1] = ControlFactory::instance("irrigation", ch);
            taskManager.StartTask(channel[ch - 1]);
            break;
        default:
            DisplayLog::PrintError("ChannelHandler: Unknow channel mode " + String(mode));
        }
    }
    static String GetChannelStatus()
    {
        String str = "[[" + String(ChannelStatus[0]) + "," + String(rom_channel[0].channel_mode) + "]," +
                     "[" + String(ChannelStatus[1]) + "," + String(rom_channel[1].channel_mode) + "]," +
                     "[" + String(ChannelStatus[2]) + "," + String(rom_channel[2].channel_mode) + "]]";
        return str;
    }
    String JsonTimer()
    {
        String ch[4];
        for (int i = 0; i < 4; i++)
        {
            String timer_list = "[";
            for (int j = 0; j < rom_channel[i].timer_size; j++)
            {
                String timer = "[" + String(rom_channel[i].timer_list[j].st) + "," + String(rom_channel[i].timer_list[j].en) + "]";
                timer_list += timer;
                if (j != (rom_channel[i].timer_size - 1))
                {
                    timer_list += ",";
                }
            }
            timer_list += "]";

            ch[i] = "{\"mode\": 1,\"timer_mode\":" + String(rom_channel[i].timer_mode) +
                    ",\"timer_list\":" + timer_list + "}";
        }
        String data = "{\"type\": \"setting\",\"data\": [" + ch[0] + "," + ch[1] + "," + ch[2] + "," + ch[3] + "]}";
        return data;
    }
    String JsonSetting()
    {
        String ch[4];
        for (int i = 0; i < 4; i++)
        {
            if (rom_channel[i].channel_mode == 0)
            {
                ch[i] = "{\"mode\": 0,\"gpio_status\":" + String(rom_channel[i].manual_status) + "}";
            }
            else if (rom_channel[i].channel_mode == 1)
            {
                String timer_list = "[";
                for (int j = 0; j < rom_channel[i].timer_size; j++)
                {
                    String timer = "[" + String(rom_channel[i].timer_list[j].st) + "," + String(rom_channel[i].timer_list[j].en) + "]";
                    timer_list += timer;
                    if (j != (rom_channel[i].timer_size - 1))
                    {
                        timer_list += ",";
                    }
                }
                timer_list += "]";

                ch[i] = "{\"mode\": 1,\"timer_mode\":" + String(rom_channel[i].timer_mode) +
                        ",\"timer_list\":" + timer_list + "}";
            }
            else if (rom_channel[i].channel_mode == 2)
            {
                ch[i] = "{\"mode\": 2,\"setpoint\":" + String(rom_channel[i].setpoint) +
                        ",\"detecting\":" + String(rom_channel[i].detecting) +
                        ",\"working\":" + String(rom_channel[i].working) +
                        ",\"sensor\":" + String(rom_channel[i].setpoint_sensor) + "}";
            }
            else if (rom_channel[i].channel_mode == 3)
            {
                ch[i] = "{\"mode\": 2,\"upper\":" + String(rom_channel[i].setbound_upper) +
                        ",\"lower\":" + String(rom_channel[i].setbound_lower) +
                        ",\"sensor\":" + String(rom_channel[i].setbound_sensor) + "}";
            }
            else if (rom_channel[i].channel_mode == 4)
            {
                ch[i] = "{\"mode\": 2,\"soil_upper\":" + String(rom_channel[i].soil_upper) +
                        ",\"soil_lower\":" + String(rom_channel[i].soil_lower) +
                        ",\"par_accum\":" + String(rom_channel[i].par_accum) +
                        ",\"mode\":" + String(rom_channel[i].irr_mode) + "}";
            }
        }
        String data = "{\"type\": \"setting\",\"data\": [" + ch[0] + "," + ch[1] + "," + ch[2] + "," + ch[3] + "]}";
        return data;
    }

  private:
    void Init()
    {
        int i = 0;
        for (i = 0; i < 4; i++)
        {
            int mode = rom_channel[i].channel_mode;
            switch (mode)
            {
            case 0:
                channel[i] = ControlFactory::instance("manual", (i + 1));
                taskManager.StartTask(channel[i]);
                break;
            case 1:
                channel[i] = ControlFactory::instance("timer", (i + 1));
                taskManager.StartTask(channel[i]);
                break;
            case 2:
                channel[i] = ControlFactory::instance("setpoint", (i + 1));
                taskManager.StartTask(channel[i]);
                break;
            case 3:
                channel[i] = ControlFactory::instance("setbound", (i + 1));
                taskManager.StartTask(channel[i]);
                break;
            case 4:
                channel[i] = ControlFactory::instance("irrigation", (i + 1));
                taskManager.StartTask(channel[i]);
                break;
            default:
                DisplayLog::PrintError("ChannelHandler: Unknow channel mode " + String(mode));
            }
        }
    }
    virtual bool OnStart()
    {
    }
    virtual void OnUpdate(uint32_t delta_time)
    {
    }
};
ChannelHanler *ChannelHanler::s_instance = 0;
// int ChannelHanler::channelStatus[4] = {OFF, OFF, OFF, OFF};
