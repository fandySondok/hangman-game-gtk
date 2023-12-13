#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "hm-conf.h"
#include "hm-tools/hm-linked-list.h"
#include "hm-debug/hm-debug.h"

int8_t hm_conf_get_words(char *filename_t, node_t **head_t)
{
  FILE *data_file = NULL;
  int try_open_file = 5;
  do
  {
    data_file = fopen(filename_t, "r");
    try_open_file--;
  } while (data_file == NULL && try_open_file > 0);

  if (data_file == NULL)
  {
    debug(__func__, "ERROR", "failed to open file %s", filename_t);
    return -1;
  }

  char buff[100];
  int16_t idx = 0;
  memset(buff, 0x00, sizeof(buff));
  char character = 0;
  while ((character = fgetc(data_file)) != EOF)
  {
    if (character > 127 || character < 9)
      break;

    if (character == '\n' || idx == (sizeof(buff) - 1))
    {
      if (memcmp(buff, "[DATA END HERE]", strlen("[DATA END HERE]")) == 0)
      {
        memset(buff, 0x00, sizeof(buff));
        idx = 0;
        fclose(data_file);
        return 0;
      }
      else if (strlen(buff) >= 20)
        goto reset_data_buff;

      if (*head_t == NULL)
      {
        *head_t = hm_lilist_create_new_node(buff);
      }
      hm_lilist_insert_to_next_ptr(head_t, (hm_lilist_create_new_node(buff)));
    reset_data_buff:;
      memset(buff, 0x00, sizeof(buff));
      idx = 0;
      continue;
    }
    buff[idx] = character;
    idx++;
  }
  fclose(data_file);
  return 1;
}