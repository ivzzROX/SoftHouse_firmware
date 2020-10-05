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

int json_get_op(const char* input, struct OP* out, struct OP_ROOT* root, struct TM* time)
{
	int counter = 0;
	int tm_counter = 0;
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

				memcpy(root[j].name, p_stt, 3);
				root[j].name[3] = 0; // manually null terminated
				root[j].id = j;

				if (root[j].name[0] == 'i') //ino
				{
					root[j].type = INO;
					root[j].operation = NULL;
					root[j].operation_n = strtol(p_stt + 4, NULL, 10) + 200; //save inp to op number
					continue;
				}

				if (root[j].name[0] == 'g') //telegram
				{
					root[j].type = TLGRM;
					root[j].operation = NULL;
					root[j].operation_n = strtol(p_stt + 4, NULL, 10) + 600;
					continue;
				}

				if (root[j].name[0] == 'p') //pmo
				{
					root[j].type = PMO;
					root[j].operation = NULL;
					root[j].operation_n = strtol(p_stt + 4, NULL, 10) + 800;
					continue;
				}

				if (root[j].name[0] == 'w') //week
				{
					root[j].type = WEEK;
					root[j].operation = NULL;
					root[j].operation_n = strtol(p_stt + 4, NULL, 16); //save value to op number
					continue;
				}

				if (root[j].name[0] == 't') //time
				{
					root[j].type = TIME;

					time[tm_counter].from.tm_hour = strtol(p_stt + 4, &p_end, 10);
					time[tm_counter].from.tm_min =  strtol(p_end + 1, &p_end, 10);

					time[tm_counter].to.tm_hour = strtol(p_end + 1, &p_end, 10);
					time[tm_counter].to.tm_min =  strtol(p_end + 1, NULL, 10);

					root[j].operation = time + tm_counter;
					root[j].operation_n = 0;

					tm_counter++;
					continue;
				}

				if (root[j].name[0] == 's' || root[j].name[0] == 'o') // state or out
				{
					root[j].type = BRCH;
					root[j].operation = NULL;
					root[j].operation_n = strtol(p_stt + 4, &p_end, 10);
					continue;
				}
			}
			root_counter = j;
			i += t[i + 1].size + 1;
		}

//		else if (jsoneq(input, &t[i], "pmo") == 0)
//		{
//			root[0].type = PMO;
//			root->operation_n = strtol(input + t[i + 1].start, NULL, 10) + 2000; //save value to op number
//			memcpy(root[0].name, "pmo\0", 4);
//			root[0].operation = NULL;
//			root[0].id = 0;
//			root[0].result = 0;
//
//			return 1;
//		}

//not implemented
//		else if (jsoneq(input, &t[i], "ino") == 0)
//		{
//			root[0].type = INO;
//			root->operation_n = strtol(input + t[i + 1].start, NULL, 10) + 200; //save value to op number
//			memcpy(root[0].name, "ino\0", 4);
//			root[0].operation = NULL;
//			root[0].id = 0;
//			root[0].result = 0;
//
//			return 1;
//		}

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
					if (memcmp(p_stt, root[r].name, 3) == 0)
					{
						root_id = root[r].id;
						break;
					}
				}

				if (root_id > 0)
				{
					out[counter].addr = 0;
					out[counter].log = (enum LG)strtol(p_stt + 5, NULL, 10);
					out[counter].root_id = root_id;
					out[counter].type = STATE;

					counter++;
					continue;
				}

				out[counter].addr = strtol(p_stt, &p_end, 16); //10
				out[counter].log = (enum LG)strtol(p_end + 1, &p_end, 10);
				out[counter].trigger_value = strtol(p_end + 1, &p_end, 10);
				out[counter].type = DATA;

				counter++;
			}
			i += t[i + 1].size + 1;
		}
	}

	return root_counter;
}
