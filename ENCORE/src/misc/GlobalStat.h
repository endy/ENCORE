#pragma once
#include "BatteryPowerWin32.h"

// power measure stuff
// base power measurement is hardcoded to test for 5 min
enum MeasureMode { NOPOWER, BASEPOWER, LOADPOWER, BUILDACPOWER, RENDERPOWER, TOTALPOWER };
MeasureMode MeasurePoint = NOPOWER;
int SleepInterval = 0;
int PowerSampleRate = 1000;
BatteryPowerWin32 BatteryMon;
DWORD StartTime;
DWORD MaxRunTime = 0;
unsigned long RenderCount = 0;