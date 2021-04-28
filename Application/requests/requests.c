/*
 * requests.c
 *
 *  Created on: 15 но€б. 2020 г.
 *      Author: D-D-E
 */

#include "requests.h"

#include "esp8266.h"
#include "engine_types.h"
#include "json_parcer.h"
#include "rtc.h"
#include "sensor.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <inttypes.h>

#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"

const char URL[] = "192.168.1.210";
const uint16_t PORT = 5002;

#define GENERATE_ENUM(ENUM, STR) ENUM,
#define GENERATE_STRING(ENUM, STR) STR,

#define FOREACH_REQ_TYPE(REQ_TYPE) \
		REQ_TYPE(GET,	"GET")   \
		REQ_TYPE(POST,	"POST")

typedef enum {
	FOREACH_REQ_TYPE(GENERATE_ENUM)
} REQ_TYPE_ENUM;

static void update_info_from_json(char* buff, struct OUTPUTS* out, uint16_t out_n)
{
	out[out_n].branch_n = -1;
	out[out_n].branch_n = json_get_op(buff, out + out_n);
}

static void clear_info(struct OUTPUTS* out, uint16_t out_n)
{
	out[out_n].branch_n = 0;
	memset(out[out_n].par, 0, sizeof(OP));
	memset(out[out_n].root_par, 0, sizeof(OP_ROOT));
}

static void create_header(char *out, REQ_TYPE_ENUM req_type, const char *url, const char *end_point)
{
	sprintf(out, "%s http://%s/%s HTTP/1.1\r\nHost: %s\r\n\r\n", req_type, url, end_point, url);
}

void ESP_GetJson()
{
	char get_t[128] = {0};
	for(uint8_t i = 1; i < OUTPUTS_N + 1; ++i)
	{
		sprintf(get_t, "GET http://192.168.1.210/test_%d HTTP/1.1\r\nHost: 192.168.1.210\r\n\r\n", i);
		ESP_SendData(URL, PORT, get_t, sizeof(get_t) - 1, 1);
		vTaskDelay(pdMS_TO_TICKS(100));

		char* ptr = NULL;
		if( (ptr = strstr(ESP_GetRxData(), "{")) != NULL )
		{
			clear_info(outputs, 0);
			update_info_from_json(ptr, outputs, i - 1);
			outputs_updated_flag = 1;
		}
	}
}

void ESP_UpdateTime()
{
	const char get_t[] = "GET http://192.168.1.210/timestamp HTTP/1.1\r\nHost: 192.168.1.210\r\n\r\n";
	ESP_SendData(URL, PORT, get_t, sizeof(get_t) - 1, 1);
	vTaskDelay(pdMS_TO_TICKS(100));

	char* ptr_tm = NULL;
	char* ptr_tz = NULL;
	if( (ptr_tm = strstr(ESP_GetRxData(), "\"tm:")) != NULL
	 && (ptr_tz = strstr(ESP_GetRxData(),  ",tz:")) != NULL )
	{
		struct tm tmp;
		long tm = strtol(ptr_tm + 4, NULL, 10);
		int8_t tz = strtol(ptr_tz + 4, NULL, 10);
		(void) localtime_r((time_t *)&tm, &tmp);
		tmp.tm_hour += tz;
		RTC_SetTime(&tmp);
	}
}

void ESP_SendSensorList()
{
	char buff[512] = { 0 };

	char prefix[] = "POST http://192.168.1.210:5002/sensors HTTP/1.1\r\nHost: 192.168.1.210\r\nContent-Type: application/json\r\nContent-Length:      \r\n\r\n{\"SENSOR\":[";
	char postfix[] = "]}";

	strcpy(buff, prefix);
	for(uint8_t i = 0; i < SENSOR_MAX; ++i)
	{
		char temp[64];
		if(sensor_list[i].addr == 0) {
			continue;
		}
		sprintf(temp, "{\"SN\": \"%04" PRIx16 "\", \"TYPE\": %d}, ", sensor_list[i].addr, sensor_list[i].type);
		strcat(buff, temp);
	}
	buff[strlen(buff) - 2] = 0;
	strcat(buff, postfix);

	char* ptr = strstr(buff, "Content-Length:");
	char* ptr_stt = strstr(buff, "\r\n\r\n");
	char* ptr_end = strstr(buff, "]}");
	char content_length[5];
	sprintf(content_length, "%d", ptr_end - ptr_stt - 2);
	memcpy(ptr + 16, content_length, strlen(content_length));

	ESP_SendData(URL, PORT, buff, sizeof(buff) - 1, 1);
}
