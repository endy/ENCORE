#include "BatteryPowerWin32.h"
#include "Common.h"

#ifdef WIN32

DWORD WINAPI ThreadProc(LPVOID lpParameter)
{
    //unsigned int *rate = (unsigned int*)lpParameter;
    BatteryPowerWin32* bp = (BatteryPowerWin32*)lpParameter;
    //cout << "size of " << sizeof(myData) << endl;
    int i = 0;
    while(true)
    {
        bp->status = CallNtPowerInformation(SystemBatteryState,0,0, &bp->bs, sizeof(SYSTEM_BATTERY_STATE));
        if(bp->status) break;
        bp->powerdata[i].dcrate = bp->bs.Rate;
        bp->powerdata[i].remain = bp->bs.RemainingCapacity;
        time(&(bp->powerdata[i].rawtime));
        bp->mysize++;
        i++;
        if(i == MAXPD)
            break;
        //bp->print();
        Sleep(bp->getSampleRate());
    }
    return 0;
}

bool BatteryPowerWin32::start(unsigned int rate)
{
    sample_rate = rate;
    m_totalTime = EncoreGetTime();
    powerdata = new myData[MAXPD];
    mysize = 0;

    if(!myhandle) 
    { 
        myhandle = CreateThread( NULL, 0, ThreadProc, this, 0, (LPDWORD)&myhandle); 
        return true; }
    return false;
}

void BatteryPowerWin32::stop(std::string filename)
{
    DWORD test;
    GetExitCodeThread(myhandle, &test);
    TerminateThread(myhandle, test);
    CloseHandle(myhandle);
    myhandle = NULL;

    m_totalTime = EncoreGetTime() - m_totalTime;
    printfile(filename);
}

void BatteryPowerWin32::printfile(std::string filename)
{
    std::vector<myData>::iterator dit; // data iterator
    FILE *f = fopen(filename.c_str(), "wt");
    unsigned int count = 1;
    if(f == NULL)
    {
        printf("Unable to open file %s to write\n",filename.c_str());
        return;
    }    
    
    fprintf(f, "discharge rate(mW),Remaining volt(mWh),time\n");
    for(int i = 0; i < mysize; i++)
    {
        fprintf(f, "%i,%i,%s", -powerdata[i].dcrate,  powerdata[i].remain, asctime (localtime ( &(powerdata[i].rawtime) )));
        count++;
    }
    fprintf(f, "stat - discharge rate\n");
    fprintf(f, "=max(a2:a%i),=min(a2:a%i),=average(a2:a%i),", count,count,count);
    fprintf(f, "\nstat - reported total power used\n");
    fprintf(f, "=(max(b2:b%i)-min(b2:b%i))", count,count);
    fprintf(f, "\nstat - calculated total power used\n");
    // average the discharge rate, multiply by milliseconds, divide by number of milliseconds in 1 hr
    fprintf(f, "=(average(a2:a%i) * %f)", count, (float)m_totalTime/3600000.0 );
	fprintf(f, "\ntotal time: %f ms\n", (float)m_totalTime);
    fclose(f);
    delete [] powerdata;
}
void BatteryPowerWin32::print()
{
    if(!status)
    {
        if(bs.Discharging && !bs.AcOnLine) 
        {
            std::cout << "discharge rate is " << -(long)bs.Rate << std::endl;
            std::cout << "estaimited time remainding " << (long)bs.EstimatedTime << " sec" << std::endl;
        }
        if(bs.Charging && bs.AcOnLine) std::cout << "charge rate is " << (long)bs.Rate << std::endl;
        std::cout << "remain/maxcapacity = " << (float)bs.RemainingCapacity/bs.MaxCapacity << std::endl;
        
    }
}

#endif

