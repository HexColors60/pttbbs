/* $Id$ */
#include <stdio.h>
#include <string.h>
#include "osdep.h"

#if defined( __FreeBSD__) || defined(_BSD_SOURCE) || __DARWIN_UNIX03
#include <stdlib.h>
// OK: Read
int
cpuload(char *str)
{
  double l[3] = {-1, -1, -1};
  // getloadavg 可以取得過去 1, 5, 15 分鐘的平均系統 loading
  if (getloadavg(l, 3) != 3)
	 l[0] = -1;

  if (str) {
   if (l[0] != -1)
    sprintf(str, " %.2f %.2f %.2f", l[0], l[1], l[2]);
   else
    strcpy(str, " (unknown) ");
  }
  // 只回傳過去一分鐘的 loading
  return (int)l[0];
}
#elif defined(__linux__)
// OK: Read
int
cpuload(char *str)
{
  double          l[3] = {-1, -1, -1};
  FILE           *fp;

  // /proc/loadavg 內存著過去 system 的 loading
  // 裡面分別有最近 1, 5, 10 分鐘的平均 loading (前三個數字)
  // 第四個數字代表正在執行與所有 processes 的數目
  // 第五個代表目前最大的 process id
  if ((fp = fopen("/proc/loadavg", "r"))) {
    if (fscanf(fp, "%lf %lf %lf", &l[0], &l[1], &l[2]) != 3)
      l[0] = -1;
    fclose(fp);
  }
  if (str) {
    if (l[0] != -1)
	    sprintf(str, " %.2f %.2f %.2f", l[0], l[1], l[2]);
    else
	    strcpy(str, " (unknown) ");
  }
  // 只回傳過去一分鐘的 loading
  return (int)l[0];
}
#else
// OK: Read
int
cpuload(char *str)
{
  // 沒有取得 loading 的手段
  strcpy(str, " (unknown) ");
  return -1;
}
#endif
