
#include "dmutil.h"
#include "dmtimermodule.h"
#include "dmsingleton.h"
#include "dmthread.h"
#include "dmconsole.h"
#include "dmtypes.h"

class CPlayer : public CDMTimerNode
{
public:
    virtual void OnTimer(unsigned long long qwIDEvent);
};

class CMain :
    public IDMConsoleSink,
    public IThread,
    public CThreadCtrl,
    public CDMTimerNode,
    public TSingleton<CMain>
{
    friend class TSingleton<CMain>;

    enum
    {
        eMAX_PLAYER = 10 * 10000,
        eMAX_PLAYER_EVENT = 10,
    };

    typedef enum
    {
        eTimerID_UUID = 0,
        eTimerID_STOP,
    }ETimerID;

    typedef enum
    {
        eTimerTime_UUID = 1000,
        eTimerTime_STOP = 10000,
    }ETimerTime;

    typedef struct tagAnyThing 
    {
        tagAnyThing()
        {
            nID = 0;
        }

        ~tagAnyThing()
        {
            nID = 0;
        }
        int nID;
        std::string strData;
    }SAnyThing;
public:

    virtual void ThrdProc()
    {
        for (int i=0; i < eMAX_PLAYER; ++i)
        {
            for (int j=1; j <= eMAX_PLAYER_EVENT; ++j)
            {
                m_oPlayers[i].SetTimer(j, 100);
            }
        }

        SetTimer(eTimerID_UUID, eTimerTime_UUID);

        SleepMs(300);

        CDMTimerModule::Instance()->Run();

		// test interface
        uint64_t qwElapse = GetTimerElapse(eTimerID_UUID);
        uint64_t qwRemain = GetTimerRemain(eTimerID_UUID);
        CDMTimerElement* poElement = GetTimerElement(eTimerID_UUID);

        bool bBusy = false;

        while(!m_bStop)
        {
            bBusy = false;
            if (CDMTimerModule::Instance()->Run())
            {
                bBusy = true;
            }

            if (__Run())
            {
                bBusy = true;
            }

            if (!bBusy)
            {
                SleepMs(1);
            }
        }
        std::cout << "test stop" << std::endl;

    }

    virtual void Terminate()
    {
        m_bStop = true;
    }

    virtual void OnCloseEvent()
    {
        Stop();
    }

    virtual void OnTimer(unsigned long long qwIDEvent, dm::any& oAny)
    {
        switch (qwIDEvent)
        {
            case eTimerID_UUID:
                {
                    std::cout << DMFormatDateTime() << " " << CMain::Instance()->GetOnTimerCount() << std::endl;
                }
                break;
            case eTimerID_STOP:
                {
                    std::cout << DMFormatDateTime() << std::endl;
                    Stop();
                }
                break;
            default:
                break;
        }
    }

    void AddOnTimerCount(){ ++m_qwOnTimerCount;}
    uint64_t GetOnTimerCount(){ return m_qwOnTimerCount;}
private:
    CMain()
        : m_bStop(false), m_qwOnTimerCount(0)
    {
        HDMConsoleMgr::Instance()->SetHandlerHook(this);
    }

    virtual ~CMain()
    {

    }

private:
    bool __Run()
    {
        return false;
    }
private:
    volatile bool   m_bStop;

    CPlayer m_oPlayers[eMAX_PLAYER];

    uint64_t  m_qwOnTimerCount;
};

void CPlayer::OnTimer(unsigned long long qwIDEvent)
{
    CMain::Instance()->AddOnTimerCount();
}

int main(int argc, char* argv[])
{
    CMain::Instance()->Start(CMain::Instance(), true);
    CMain::Instance()->WaitFor();
    return 0;
}