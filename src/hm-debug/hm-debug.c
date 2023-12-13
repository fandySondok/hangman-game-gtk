#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <stdint.h>
#include <stdarg.h>

#include "hm-debug.h"

int debug(const char *function_name, char *debug_type, char *debug_msg, ...)
{
  struct timeval tv;
  gettimeofday(&tv, NULL);
  struct tm *d_tm;
  d_tm = localtime(&tv.tv_sec);
  uint16_t msec = tv.tv_usec / 1000;
  char time_str[24];
  memset(time_str, 0x00, 24 * sizeof(char));
  sprintf(time_str, "%04d-%02d-%02d %02d:%02d:%02d.%.3i", d_tm->tm_year + 1900, d_tm->tm_mon + 1,
          d_tm->tm_mday, d_tm->tm_hour, d_tm->tm_min, d_tm->tm_sec, msec);

  char tmp_debug_msg[125];
  memset(tmp_debug_msg, 0x00, 125 * sizeof(char));
  va_list aptr;
  va_start(aptr, debug_msg);
  vsprintf(tmp_debug_msg, debug_msg, aptr);
  va_end(aptr);

  if (strstr(debug_type, "INFO"))
  {
    printf("%sI:%s %s %s%s:%s %s\n", BGgreen, KNRM, time_str, BOLDGREEN, function_name, KNRM, tmp_debug_msg);
  }
  else if (strstr(debug_type, "ERROR"))
  {
    printf("%sE:%s %s %s%s:%s %s\n", BGred, KNRM, time_str, BGred, function_name, KNRM, tmp_debug_msg);
  }
  else if (strstr(debug_type, "WARNING"))
  {
    printf("%sW:%s %s %s%s:%s %s\n", BGyellow, KNRM, time_str, BOLDYELLOW, function_name, KNRM, tmp_debug_msg);
  }
  return 0;
}