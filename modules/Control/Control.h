#include <Task.h>
extern TaskManager taskManager;
class Control : public Task
{
  public:
    Control(int ms,int ch) : Task(MsToTaskTime(ms))
      {
        channel = ch;
      };

  protected:
    int channel;
    virtual void SetValue()
    {
      
    }
  private:
    
    virtual bool OnStart()
    {

    }
    virtual void OnUpdate(uint32_t delta_time)
    {
      Serial.println("Control is running " + String(channel));
    }
};