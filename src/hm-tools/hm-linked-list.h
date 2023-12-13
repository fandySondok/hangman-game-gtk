#ifndef HM_LINKED_LIST_H
#define HM_LINKED_LIST_H

struct node
{
  char value[50];
  struct node *next;
};

typedef struct node node_t;

node_t *hm_lilist_create_new_node(char *data_t);
void hm_lilist_insert_to_next_ptr(node_t **tmp_head, node_t *tmp_t);
void hm_lilist_print(node_t *head);
int hm_lilist_find_node(node_t *tmp_head, char *tmp_value);
void hm_lilist_delete_node(node_t **head);
node_t *hm_lilist_find_node_with_id(node_t *tmp_head, int node_c);
int hm_lilist_count_node(node_t *tmp_head);

#endif