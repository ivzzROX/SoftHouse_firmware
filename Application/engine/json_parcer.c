#include "json_parcer.h"
#include "jsmn.h"

#include <string.h>
#include <stdlib.h>

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

		else if (jsoneq(input, &t[i], "brch") == 0)
		{
			if (t[i + 1].type != JSMN_ARRAY)
			{
				continue;
			}

			int j = 0;
			for (j; j < t[i + 1].size; ++j)
			{
				jsmntok_t* g = &t[i + j + 2];

				char* p_stt = (char*)input + g->start + 1;
				char* p_end = NULL;

				memcpy(out->root_par[j].name, p_stt, 3);
				out->root_par[j].name[3] = 0; //manually null terminated
				out->root_par[j].id = j;

				if (out->root_par[j].name[0] == 'i') //ino
				{
					out->root_par[j].type = INO;
					out->root_par[j].operation = NULL;
					out->root_par[j].operation_n = strtol(p_stt + 4, NULL, 10) + 200; //save inp to op number
					continue;
				}

				if (out->root_par[j].name[0] == 'g') //telegram
				{
					out->root_par[j].type = WEB;
					out->root_par[j].operation = NULL;
					out->root_par[j].operation_n = strtol(p_stt + 4, NULL, 10) + 600;
					continue;
				}

				if (out->root_par[j].name[0] == 'p') //pmo
				{
					out->root_par[j].type = PMO;
					out->root_par[j].operation = NULL;
					out->root_par[j].operation_n = strtol(p_stt + 4, NULL, 10) + 800;
					continue;
				}

				if (out->root_par[j].name[0] == 'w') //week
				{
					out->root_par[j].type = WEEK;
					out->root_par[j].operation = NULL;
					out->root_par[j].operation_n = strtol(p_stt + 4, NULL, 16); //save value to op number
					continue;
				}

				if (out->root_par[j].name[0] == 'c') //counter
				{
					out->root_par[j].type = COUNTER;

					for (uint16_t r = 0; r < j; ++r)
					{
						if (memcmp(p_stt + 5, out->root_par[r].name, 3) == 0)
						{
							out->cntr[cntr_counter].root_id = out->root_par[r].id;
							break;
						}
					}
					out->cntr[cntr_counter].trigger_value = strtol(p_stt + 10, NULL, 10);

					out->root_par[j].operation = out->cntr + cntr_counter;
					out->root_par[j].operation_n = 1;

					cntr_counter++;
					continue;
				}

				if (out->root_par[j].name[0] == 'd') //delay
				{
					out->root_par[j].type = DELAY;

					for (uint16_t r = 0; r < j; ++r)
					{
						if (memcmp(p_stt + 5, out->root_par[r].name, 3) == 0)
						{
							out->del[dl_counter].root_id = out->root_par[r].id;
							break;
						}
					}
					out->del[dl_counter].value = strtol(p_stt + 10, NULL, 10);

					out->root_par[j].operation = out->del + dl_counter;
					out->root_par[j].operation_n = 1;

					dl_counter++;
					continue;
				}

				if (out->root_par[j].name[0] == 'f') //t trigger
				{
					out->root_par[j].type = T_TRIGGER;

					for (uint16_t r = 0; r < j; ++r)
					{
						if (memcmp(p_stt + 5, out->root_par[r].name, 3) == 0)
						{
							out->t_tr[tt_counter].root_id = out->root_par[r].id;
							break;
						}
					}

					out->root_par[j].operation = out->t_tr + tt_counter;
					out->root_par[j].operation_n = 1;

					tt_counter++;
					continue;
				}

				if (out->root_par[j].name[0] == 'r') //rs trigger
				{
					out->root_par[j].type = RS_TRIGGER;

					for (uint16_t r = 0; r < j; ++r)
					{
						if (memcmp(p_stt + 5, out->root_par[r].name, 3) == 0)
						{
							out->rs_tr[rst_counter].root_id_s = out->root_par[r].id;
							break;
						}
					}

					for (uint16_t r = 0; r < j; ++r)
					{
						if (memcmp(p_stt + 10, out->root_par[r].name, 3) == 0)
						{
							out->rs_tr[rst_counter].root_id_r = out->root_par[r].id;
							break;
						}
					}

					out->root_par[j].operation = out->rs_tr + rst_counter;
					out->root_par[j].operation_n = 1;

					rst_counter++;
					continue;
				}

				if (out->root_par[j].name[0] == 't') //time
				{
					out->root_par[j].type = TIME;

					out->tim[tm_counter].from.tm_hour = strtol(p_stt + 4, &p_end, 10);
					out->tim[tm_counter].from.tm_min =  strtol(p_end + 1, &p_end, 10);

					out->tim[tm_counter].to.tm_hour = strtol(p_end + 1, &p_end, 10);
					out->tim[tm_counter].to.tm_min =  strtol(p_end + 1, NULL, 10);

					out->root_par[j].operation = out->tim + tm_counter;
					out->root_par[j].operation_n = 1;

					tm_counter++;
					continue;
				}

				if (out->root_par[j].name[0] == 's' || out->root_par[j].name[0] == 'o') //state or out
				{
					out->root_par[j].type = BRCH;
					out->root_par[j].operation = NULL;
					out->root_par[j].operation_n = strtol(p_stt + 4, &p_end, 10);
					continue;
				}
			}
			root_counter = j;
			i += t[i + 1].size + 1;
		}
		else
		{
			if (t[i + 1].type != JSMN_ARRAY)
			{
				continue;
			}

			for (int j = 0; j < t[i + 1].size; j++)
			{
				jsmntok_t* g = &t[i + j + 2];
				char* p_stt = (char*)input + g->start + 1;
				char* p_end = NULL;
				int root_id = 0;

				for (int r = 0; r < root_counter; ++r)
				{
					if (memcmp(p_stt, out->root_par[r].name, 3) == 0)
					{
						root_id = out->root_par[r].id;
						break;
					}
				}

				if (root_id > 0)
				{
					out->par[counter].addr = 0;
					out->par[counter].log = (enum LG)strtol(p_stt + 5, NULL, 10);
					out->par[counter].root_id = root_id;
					out->par[counter].type = STATE;

					counter++;
					continue;
				}

				out->par[counter].addr = strtol(p_stt, &p_end, 16); //10
				out->par[counter].log = (enum LG)strtol(p_end + 1, &p_end, 10);
				out->par[counter].trigger_value = strtol(p_end + 1, &p_end, 10);
				out->par[counter].type = DATA;

				counter++;
			}
			i += t[i + 1].size + 1;
		}
	}

	return root_counter;
}
