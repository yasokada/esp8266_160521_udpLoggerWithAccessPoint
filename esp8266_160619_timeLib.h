#ifndef ESP8266_160619_TIMELIB_H
#define ESP8266_160619_TIMELIB_H

void TimeLib_Set_yyyymmddhhnnss(char *srcPtr, int size);
void Test_TimeLib_Set_yyyymmddhhnnss(void);
void TimeLib_SerialTxDateTime(void);
void TimeLib_copyDateTimeTo(char *dstPtr, int size);

#endif

