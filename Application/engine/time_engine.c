#define _CRT_SECURE_NO_WARNINGS
#include "time_engine.h"
#include "rtc.h"

#include <time.h>

uint8_t is_cur_time(TM* ft)
{
	struct tm time_info = { 0 };
	RTC_GetTime(&time_info);

	if (time_info.tm_hour == ft->from.tm_hour &&
		time_info.tm_min >= ft->from.tm_min &&
		time_info.tm_hour < ft->to.tm_hour)
	{
		return 1;
	}

	if (time_info.tm_hour > ft->from.tm_hour &&
		time_info.tm_hour < ft->to.tm_hour)
	{
		return 1;
	}

	if (time_info.tm_hour == ft->to.tm_hour &&
		time_info.tm_min <= ft->to.tm_min)
	{
		return 1;
	}

	return 0;
}

uint8_t is_cur_day_of_week(uint16_t data)
{
	struct tm time_info = { 0 };
	RTC_GetTime(&time_info);

	if (data & (1 << time_info.tm_wday))
	{
		return 1;
	}
	return 0;
}
