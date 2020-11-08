#include "json_parcer.h"
#include "jsmn.h"

#include <string.h>
#include <stdlib.h>
#include <ctype.h>

static int jsoneq(const char* json, jsmntok_t* tok, const char* s) {
	if (tok->type == JSMN_STRING && (int)strlen(s) == tok->end - tok->start &&
		strncmp(json + tok->start, s, tok->end - tok->start) == 0) {
		return 0;
	}
	return -1;
}

int json_get_op(const char* input, struct OUTPUTS* out)
{
	int counter = 0;
	int tm_counter = 0;
	int tt_counter = 0;
	int rst_counter = 0;
	int cntr_counter = 0;
	int dl_counter = 0;
	int root_counter = 0;
	int i;
	int r;
	jsmn_parser p;
	jsmntok_t t[128];

	jsmn_init(&p);
	r = jsmn_parse(&p, input, strlen(input), t, sizeof(t) / sizeof(t[0]));

	if (r < 1 || t[0].type != JSMN_OBJECT)
	{
		return 0;
	}

	for (i = 1; i < r; i++) {

		if (jsoneq(input, &t[i], "OUT") == 0)
		{
			i++;
		}
		else
		{
			if (t[i + 1].type != JSMN_ARRAY)
			{
				continue;
			}

			//add new branch
			memcpy(out->root_par[root_counter].name, input + t[i].start, t[i].end - t[i].start);
			out->root_par[root_counter].operation = NULL;
			out->root_par[root_counter].operation_n = 0;
			out->root_par[root_counter].id = root_counter;

			//parse branch
			for (int j = 0; j < t[i + 1].size; j++)
			{
				out->root_par[root_counter].operation_n++;

				jsmntok_t* g = &t[i + j + 2];
				char* p_stt = (char*)input + g->start + 1;
				char* p_end = NULL;


				if (isalpha(p_stt[0]) && islower(p_stt[0]))
				{
					out->par[counter].root_id = 0;
					switch(p_stt[0])
					{
					case 's': //another branch
						out->par[counter].type = STATE;
						for (uint16_t r = 0; r < root_counter; ++r) {
							if (memcmp(p_stt, out->root_par[r].name, 3) == 0) {
								out->par[counter].root_id = out->root_par[r].id;
								break;
							}
						}
						out->par[counter].log = (enum LG)strtol(p_stt + 5, NULL, 16);
						break;

					case 'g': //telegram
						out->par[counter].type = WEB;
						out->par[counter].log = (enum LG)strtol(p_end + 3, &p_end, 16);
						out->par[counter].trigger_value = strtol(p_end + 1, NULL, 10);
						break;

					case 'p': //pmo
						out->par[counter].type = PMO;
						out->par[counter].log = (enum LG)strtol(p_end + 3, &p_end, 16);
						out->par[counter].trigger_value = strtol(p_end + 1, NULL, 10);
						break;

					case 'i': //ino
						out->par[counter].type = INO;
						out->par[counter].log = (enum LG)strtol(p_end + 3, &p_end, 16);
						out->par[counter].trigger_value = strtol(p_end + 1, NULL, 10);
						break;

					case 'w': //week
						out->par[counter].type = WEEK;
						out->par[counter].log = (enum LG)strtol(p_end + 3, &p_end, 16);
						out->par[counter].trigger_value = strtol(p_end + 1, NULL, 16);
						break;

					case 'c': //counter
						out->par[counter].type = COUNTER;
						out->par[counter].log = (enum LG)strtol(p_stt + 3, NULL, 16);

						if (memcmp(p_stt + 6, "psv", 3) == 0) {
							(out->cntr + cntr_counter)->root_id = -1;
						} else {
							for (uint16_t r = 0; r < root_counter; ++r) {
								if (memcmp(p_stt + 6, out->root_par[r].name, 3) == 0) {
									(out->cntr + cntr_counter)->root_id = out->root_par[r].id;
									break;
								}
							}
						}

						out->par[counter].trigger_value = strtol(p_stt + 11, NULL, 10);
						out->par[counter].data = out->cntr + cntr_counter;
						cntr_counter++;
						break;

					case 'd': //delay
						out->par[counter].type = DELAY;
						out->par[counter].log = (enum LG)strtol(p_stt + 3, NULL, 16);

						if (memcmp(p_stt + 6, "psv", 3) == 0) {
							(out->del + dl_counter)->root_id = -1;
						} else {
							for (uint16_t r = 0; r < root_counter; ++r) {
								if (memcmp(p_stt + 6, out->root_par[r].name, 3) == 0) {
									(out->del + dl_counter)->root_id = out->root_par[r].id;
									break;
								}
							}
						}

						out->del[dl_counter].value = strtol(p_stt + 11, NULL, 10);
						out->par[counter].data	= out->del + dl_counter;
						dl_counter++;
						break;

					case 'f': //t trigger
						out->par[counter].type = T_TRIGGER;
						out->par[counter].log = (enum LG)strtol(p_stt + 3, NULL, 16);

						if (memcmp(p_stt + 6, "psv", 3) == 0) {
							(out->t_tr + tt_counter)->root_id = -1;
						} else {
							for (uint16_t r = 0; r < root_counter; ++r) {
								if (memcmp(p_stt + 6, out->root_par[r].name, 3) == 0) {
									(out->t_tr + tt_counter)->root_id = out->root_par[r].id;
									break;
								}
							}
						}

						out->par[counter].data	= out->t_tr + tt_counter;
						tt_counter++;
						break;

					case 'r': //rs trigger
						out->par[counter].type = RS_TRIGGER;
						out->par[counter].log = (enum LG)strtol(p_stt + 3, NULL, 16);

						if (memcmp(p_stt + 6, "psv", 3) == 0) {
							out->rs_tr[rst_counter].root_id_s = -1;
						} else {
							for (uint16_t r = 0; r < root_counter; ++r) {
								if (memcmp(p_stt + 6, out->root_par[r].name, 3) == 0) {
									out->rs_tr[rst_counter].root_id_s = out->root_par[r].id;
									break;
								}
							}
						}

						if (memcmp(p_stt + 6, "psv", 3) == 0) {
							out->rs_tr[rst_counter].root_id_r = -1;
						} else {
							for (uint16_t r = 0; r < root_counter; ++r) {
								if (memcmp(p_stt + 11, out->root_par[r].name, 3) == 0) {
									out->rs_tr[rst_counter].root_id_r = out->root_par[r].id;
									break;
								}
							}
						}

						out->par[counter].data	= out->rs_tr + rst_counter;
						rst_counter++;
						break;

					case 't': //time
						out->par[counter].type = TIME;
						out->par[counter].log = (enum LG)strtol(p_stt + 3, NULL, 16);
						out->tim[tm_counter].from.tm_hour = strtol(p_stt + 8, &p_end, 10);
						out->tim[tm_counter].from.tm_min =  strtol(p_end + 1, &p_end, 10);
						out->tim[tm_counter].to.tm_hour = strtol(p_end + 1, &p_end, 10);
						out->tim[tm_counter].to.tm_min =  strtol(p_end + 1, NULL, 10);
						out->par[counter].data	= out->tim + tm_counter;
						tm_counter++;
						break;

					default:
						break;
					}
					counter++;
					continue;
				}

				out->par[counter].addr = strtol(p_stt, &p_end, 16);
				out->par[counter].log = (enum LG)strtol(p_end + 1, &p_end, 10);
				out->par[counter].trigger_value = strtol(p_end + 1, &p_end, 10);
				out->par[counter].type = SENSOR;

				counter++;
			}
			out->root_par[root_counter].operation = out->par + (counter - out->root_par[root_counter].operation_n);
			root_counter++;
			i += t[i + 1].size + 1;
		}
	}

	return root_counter;
}
