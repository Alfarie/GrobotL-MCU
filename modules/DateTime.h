#include <Task.h>
#include <Wire.h>
extern TaskManager taskManager;

#define ADDRESS 0x68

struct DT
{
    byte day;
    byte dayOfWeek;
    byte month;
    byte year;
    byte second;
    byte minute;
    byte hour;
};

class DateTime : public Task
{
  public:
    static DateTime *s_instance;
    DateTime() : Task(MsToTaskTime(500)){};

    static DateTime *instance()
    {
        if (!s_instance)
            s_instance = new DateTime();
        return s_instance;
    }

    int GetCurrentMin()
    {
        return _datetime.hour * 60 + _datetime.minute;
    }

    String GetDateTimeString()
    {
        String str = AddZero(_datetime.hour) + String(_datetime.hour) + ":" +
                     AddZero(_datetime.minute) + String(_datetime.minute) + ":" +
                     AddZero(_datetime.second) + String(_datetime.second) + " " +

                     String(_datetime.day) + "/" +
                     String(_datetime.month) + "/" +
                     String(_datetime.year);
        return str;
    }

    String GetDateString()
    {
        // 2014-08-20 15:30:00
        String str = "20" + String(_datetime.year) + "-" + String(_datetime.month) + "-" + String(_datetime.day);
        return str;
    }
    
    String GetTimeString()
    {
        String str = AddZero(_datetime.hour) + String(_datetime.hour) + ":" +
                     AddZero(_datetime.minute) + String(_datetime.minute) + ":" +
                     AddZero(_datetime.second) + String(_datetime.second);
        return str;
    }

    void setDateDs1307(byte s, byte m, byte h, byte dow, byte dom, byte mo, byte y)
    {
        Wire.beginTransmission(ADDRESS);
        Wire.write(decToBcd(0));
        Wire.write(decToBcd(s)); // 0 to bit 7 starts the clock
        Wire.write(decToBcd(m));
        Wire.write(decToBcd(h)); // If you want 12 hour am/pm you need to set
        // bit 6 (also need to change readDateDs1307)
        Wire.write(decToBcd(dow));
        Wire.write(decToBcd(dom));
        Wire.write(decToBcd(mo));
        Wire.write(decToBcd(y));
        Wire.endTransmission();
        Refresh();
        Serial.println(GetDateString());
    }

    void Refresh()
    {
        Wire.beginTransmission(ADDRESS);
        Wire.write(decToBcd(0));
        Wire.endTransmission();
        Wire.requestFrom(ADDRESS, 7);

        _datetime.second = bcdToDec(Wire.read() & 0x7f);
        _datetime.minute = bcdToDec(Wire.read());
        _datetime.hour = bcdToDec(Wire.read() & 0x3f); // Need to change this if 12 hour am/pm
        _datetime.dayOfWeek = bcdToDec(Wire.read());
        _datetime.day = bcdToDec(Wire.read());
        _datetime.month = bcdToDec(Wire.read());
        _datetime.year = bcdToDec(Wire.read());
    }
    
	void ShowDateTime(DT dt)
    {
        // 12:00:00 12/12/17
        String str = "[PlantLab] Date Time: " + String(dt.hour) + ":" + String(dt.minute) + ":" + String(dt.second) + " " + String(dt.day) + "/" + String(dt.month) + "/" + String(dt.year);
        Serial.println(str);
    }

	void CheckRunning() //this function check clock status and automatically starts clock if stops.
	{
		Wire.beginTransmission(ADDRESS);
		Wire.write(decToBcd(0));
		Wire.endTransmission();
		Wire.requestFrom(ADDRESS, 1);
		byte k = Wire.read();
		if((k&0b10000000) == 0x00)
		{
			return;
		}
		else
		{
			Wire.beginTransmission(ADDRESS);
			Wire.write(decToBcd(0));
			Wire.write(k&0b01111111); // set 0 to bit 7 of second starts the clock
			Wire.endTransmission();
			return;
		}
	}

  private:
    DT _datetime = {0,0,0,0,0,0,0};
    virtual bool OnStart()
    {
        Wire.begin();
        Serial.println("[Info] DateTime instance initialized...");
        Refresh();
        ShowDateTime(_datetime);
        delay(200);
    }
    virtual void OnUpdate(uint32_t delta_time)
    {
        // ShowDateTime(_datetime);
        Refresh();
        CheckRunning();
    }

    byte decToBcd(byte val)
    {
        return ((val / 10 * 16) + (val % 10));
    }
    byte bcdToDec(byte val)
    {
        return ((val / 16 * 10) + (val % 16));
    }
    
    String AddZero(byte val)
    {
        if (val < 10)
        {
            return "0";
        }
        else
            return "";
    }
};
DateTime *DateTime::s_instance = 0;