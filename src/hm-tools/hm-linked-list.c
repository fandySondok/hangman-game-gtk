#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hm-linked-list.h"

node_t *hm_lilist_create_new_node(char *data_t)
{
  node_t *result = malloc(sizeof(node_t));
  memset(result, 0x00, sizeof(node_t));
  memcpy(result->value, data_t, sizeof(result->value));
  result->next = NULL;
  return result;
}

void hm_lilist_insert_to_next_ptr(node_t **tmp_head, node_t *tmp_t)
{
  tmp_t->next = *tmp_head;
  *tmp_head = tmp_t;
}

void hm_lilist_print(node_t *head)
{
  node_t *temporary = head;
  while (temporary != NULL)
  {
    printf("%s - ", temporary->value);
    temporary = temporary->next;
  }
  printf("\n");
}

int hm_lilist_find_node(node_t *tmp_head, char *tmp_value)
{
  node_t *temporary = tmp_head;
  while (temporary != NULL)
  {
    if (memcmp(temporary->value, tmp_value, strlen(tmp_value)) == 0)
      return 0;
    temporary = temporary->next;
  }
  free(temporary);
  return -1;
}

void hm_lilist_delete_node(node_t **head)
{
  node_t *current = *head;
  node_t *next;
  while (current != NULL)
  {
    next = current->next;
    free(current);
    current = next;
  }
  *head = NULL;
}

node_t *hm_lilist_find_node_with_id(node_t *tmp_head, int node_c)
{
  node_t *temporary = tmp_head;
  int tmp_node_c = 0;
  while (temporary != NULL)
  {
    tmp_node_c++;
    if (tmp_node_c == node_c)
      return temporary;
    temporary = temporary->next;
  }
  return NULL;
}

int hm_lilist_count_node(node_t *tmp_head)
{
  int tmp_node_c = 0;
  node_t *temporary = tmp_head;
  while (temporary != NULL)
  {
    temporary = temporary->next;
    tmp_node_c++;
  }
  free(temporary);
  return tmp_node_c;
}