#include <TimeLib.h>
#include <stdarg.h> // for va_XXX()

/*
 * using time library by
 * https://github.com/PaulStoffregen/Time
 */

/*
 * v0.2 19 Jun. 2016
 *   - add TimeLib_copyDateTimeTo()
 *   - add myCopyFunc()
 * v0.1 19 Jun. 2016
 *   - add TimeLib_SerialTxDateTime()
 *   - add myPrintf()
 *   - add Test_TimeLib_Set_yyyymmddhhnnss()
 *   - add TimeLib_Set_yyyymmddhhnnss()
 */

void TimeLib_Set_yyyymmddhhnnss(char *srcPtr, int size)
{
  //         01234567890123
  // srcPtr: yyyymmddhhnnss

  if (size < 14) {
    return; // error
  }
  char szbuf[20];
  int hh, nn, ss, dd, mm, yyyy;

  strcpy(szbuf, srcPtr);
  ss = atoi(&szbuf[12]);
  szbuf[12] = 0x00;
  nn = atoi(&szbuf[10]);
  szbuf[10] = 0x00;
  hh = atoi(&szbuf[8]);
  szbuf[8] = 0x00;
  dd = atoi(&szbuf[6]);
  szbuf[6] = 0x00;
  mm = atoi(&szbuf[4]);
  szbuf[4] = 0x00;
  yyyy = atoi(&szbuf[0]);

  setTime(hh, nn, ss, dd, mm, yyyy);
}

void Test_TimeLib_Set_yyyymmddhhnnss(void)
{
  char settim[] = "20160619101200"; // yyyymmddhhnnss
  TimeLib_Set_yyyymmddhhnnss(settim, sizeof(settim));  
}

void myPrintf(char *fmt, ...) 
{
  char buf[128];
  va_list args;
  va_start(args, fmt);
  vsnprintf(buf, 128, fmt, args);
  va_end(args);
  Serial.print(buf);  
}

void TimeLib_SerialTxDateTime(void)
{
  myPrintf("%02d:%02d:%02d", hour(), minute(), second() );
  myPrintf(",%02d/%02d/%04d", day(), month(), year() );
  Serial.println();
}

void myCopyFunc(char *dstPtr, char *fmt, ...)
{
  if (dstPtr == NULL) {
    return;
  }
  va_list args;
  va_start(args, fmt);
  vsnprintf(dstPtr, 128, fmt, args);
  va_end(args);  
}

void TimeLib_copyDateTimeTo(char *dstPtr, int size)
{
  if (dstPtr == NULL) {
    return;
  }

  if (size < 19) {
    return;
  }

  // 0123456789012345678
  // 12:30:45,19/06/2016

  myCopyFunc(dstPtr, "%02d:%02d:%02d,%02d/%02d/%04d,",
    hour(), minute(), second(), 
    day(), month(), year()
    );

}