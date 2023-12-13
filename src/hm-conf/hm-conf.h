#ifndef HM_CONF_H
#define HM_CONF_H

#include "hm-tools/hm-linked-list.h"

typedef struct node node_t;

int8_t hm_conf_get_words(char *filename_t, node_t **head_t);

#endif