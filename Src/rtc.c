#include "rtc.h"

void RTC_Init(void)
{
	LL_RTC_InitTypeDef RTC_InitStruct = {0};
	LL_RTC_TimeTypeDef RTC_TimeStruct = {0};
	LL_RTC_DateTypeDef RTC_DateStruct = {0};

	LL_RCC_EnableRTC();

	RTC_InitStruct.HourFormat = LL_RTC_HOURFORMAT_24HOUR;
	RTC_InitStruct.AsynchPrescaler = 127;
	RTC_InitStruct.SynchPrescaler = 255;
	LL_RTC_Init(RTC, &RTC_InitStruct);
	LL_RTC_SetAsynchPrescaler(RTC, 127);
	LL_RTC_SetSynchPrescaler(RTC, 255);

	if(LL_RTC_BAK_GetRegister(RTC, LL_RTC_BKP_DR0) != 0x32F2)
	{
		RTC_TimeStruct.Hours = 0;
		RTC_TimeStruct.Minutes = 0;
		RTC_TimeStruct.Seconds = 0;
		LL_RTC_TIME_Init(RTC, LL_RTC_FORMAT_BCD, &RTC_TimeStruct);
		RTC_DateStruct.WeekDay = LL_RTC_WEEKDAY_MONDAY;
		RTC_DateStruct.Month = LL_RTC_MONTH_JANUARY;
		RTC_DateStruct.Year = 0;
		LL_RTC_DATE_Init(RTC, LL_RTC_FORMAT_BCD, &RTC_DateStruct);
		LL_RTC_BAK_SetRegister(RTC,LL_RTC_BKP_DR0,0x32F2);
	}

}

void RTC_GetTime(struct tm* time)
{
	time->tm_hour = __LL_RTC_CONVERT_BCD2BIN(LL_RTC_TIME_GetHour(RTC));
	time->tm_min = 	__LL_RTC_CONVERT_BCD2BIN(LL_RTC_TIME_GetMinute(RTC));
	time->tm_sec = 	__LL_RTC_CONVERT_BCD2BIN(LL_RTC_TIME_GetSecond(RTC));

	time->tm_mday = __LL_RTC_CONVERT_BCD2BIN(LL_RTC_DATE_GetDay(RTC));
	time->tm_mon = __LL_RTC_CONVERT_BCD2BIN(LL_RTC_DATE_GetMonth(RTC)) - 1;
	time->tm_year = __LL_RTC_CONVERT_BCD2BIN(LL_RTC_DATE_GetYear(RTC));

	// RTC HAS DIF WEEK DAY MASKS
	//tm  0 - sunday 6 - saturday
	//rtc 1 - monday 7 - sunday

	switch(LL_RTC_DATE_GetWeekDay(RTC))
	{
	case LL_RTC_WEEKDAY_MONDAY: 	time->tm_wday = 1; break;
	case LL_RTC_WEEKDAY_TUESDAY: 	time->tm_wday = 2; break;
	case LL_RTC_WEEKDAY_WEDNESDAY: 	time->tm_wday = 3; break;
	case LL_RTC_WEEKDAY_THURSDAY: 	time->tm_wday = 4; break;
	case LL_RTC_WEEKDAY_FRIDAY: 	time->tm_wday = 5; break;
	case LL_RTC_WEEKDAY_SATURDAY: 	time->tm_wday = 6; break;
	case LL_RTC_WEEKDAY_SUNDAY: 	time->tm_wday = 0; break;
	default: time->tm_wday = 0; break;
	}
}

void RTC_SetTime(struct tm* time)
{
	LL_RTC_DisableWriteProtection(RTC);
	LL_RTC_EnterInitMode(RTC);

	LL_RTC_TimeTypeDef RTC_TimeStruct = {0};
	LL_RTC_DateTypeDef RTC_DateStruct = {0};

	RTC_TimeStruct.Hours = time->tm_hour;
	RTC_TimeStruct.Minutes = time->tm_min;
	RTC_TimeStruct.Seconds = time->tm_sec;
	LL_RTC_TIME_Init(RTC, LL_RTC_FORMAT_BIN, &RTC_TimeStruct);

	switch(time->tm_wday)
	{
	case 0:	RTC_DateStruct.WeekDay = LL_RTC_WEEKDAY_SUNDAY;		break;
	case 1:	RTC_DateStruct.WeekDay = LL_RTC_WEEKDAY_MONDAY; 	break;
	case 2:	RTC_DateStruct.WeekDay = LL_RTC_WEEKDAY_TUESDAY; 	break;
	case 3: RTC_DateStruct.WeekDay = LL_RTC_WEEKDAY_WEDNESDAY; 	break;
	case 4:	RTC_DateStruct.WeekDay = LL_RTC_WEEKDAY_THURSDAY; 	break;
	case 5:	RTC_DateStruct.WeekDay = LL_RTC_WEEKDAY_FRIDAY; 	break;
	case 6:	RTC_DateStruct.WeekDay = LL_RTC_WEEKDAY_SATURDAY;	break;
	default: RTC_DateStruct.WeekDay = LL_RTC_WEEKDAY_SUNDAY; 	break;
	}

	RTC_DateStruct.Day = time->tm_mday;
	RTC_DateStruct.Month = time->tm_mon + 1;
	RTC_DateStruct.Year = time->tm_year;
	LL_RTC_DATE_Init(RTC, LL_RTC_FORMAT_BIN, &RTC_DateStruct);

	LL_RTC_DisableInitMode(RTC);
	LL_RTC_EnableWriteProtection(RTC);
}
