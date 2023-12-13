#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>

#include "hm-gui-main.h"
#include "hm-debug/hm-debug.h"
#include "hm-gui-menu.h"
#include "hm-conf/hm-conf.h"
#include "hm-tools/hm-linked-list.h"
#include "hm-tools/hm-tools.h"
#include "hm-gui-popup.h"

#define LBL_LEN 5
#define FILE_WORDS "conf/list-of-words.txt"
typedef struct
{
  GtkWidget *window;
  GtkWidget *container;
  GtkWidget *answer_box;
  GtkWidget *anws_label_colomn[20];
  GtkWidget *back_to_menu_box;
  GtkWidget *back_to_menu_button;
  GtkWidget *animation_box;
  GtkWidget *animation;
  GtkWidget *keyboard_box;
  GtkWidget *keyboard_container;

  GtkWidget *btn_[26];
} widget_window_main_t;
typedef enum
{
  START_GAME,
  SHOW_DATA,
  CHECK_WON_OR_NOT,
  GUESS_DATA,
  SUCCESS,
  FAIL,
  WAIT_FOR_CHANGE_STATE
} main_st;

typedef enum
{
  WIN_IMG = 9,
  WAIT_FOR_SHOW_IMG = 8,
  START_IMG = 7,
  SIX_REMAIN_IMG = 6,
  FIVE_REMAIN_IMG = 5,
  FOUR_REMAIN_IMG = 4,
  THREE_REMAIN_IMG = 3,
  TWO_REMAIN_IMG = 2,
  ONE_REMAIN_IMG = 1,
  LOSE_IMG = 0
} img_st;

static void hm_ui_window_main_get_object();
static void hm_ui_window_main_signal_connect();

gboolean ui_update_main(gpointer not_used);
void *main_thread(void *ptr);
static gboolean hm_ui_window_main_keypress(GtkWidget *_widget_, GdkEventKey *_event_, gpointer _data_);
void hm_ui_window_main_won_game();
void hm_ui_window_main_lost_game();

char lbl_anwr[20][LBL_LEN];
char lbl_anwr_tmp[20][LBL_LEN];
int flag_data_show = 0;
gboolean flag_popup_show = FALSE;
int lives = 7;

static widget_window_main_t ui_widget;
GMutex lock_screen_main;
GMutex lock_set_text;
struct timeval tvLastAccessSTCmdMain = {0, 0};
gboolean signal_main_flag = TRUE;
gboolean signal_close_update_main_loop = FALSE;
gboolean flag_reset_sensitivity_btn = FALSE;
pthread_t th_main_window;

main_st main_state_thread = START_GAME;

img_st main_img_state = WAIT_FOR_SHOW_IMG;

void hm_ui_window_main_initialize()
{
  g_mutex_init(&lock_screen_main);
  g_mutex_init(&lock_set_text);
  hm_ui_window_main_get_object();
  hm_ui_window_main_signal_connect();
  gtk_window_set_destroy_with_parent(GTK_WINDOW(ui_widget.window), TRUE);
}

void hm_ui_window_main_deinitialize()
{
  signal_close_update_main_loop = TRUE;
  g_mutex_clear(&lock_screen_main);
  g_mutex_clear(&lock_set_text);
  gtk_window_close(GTK_WINDOW(ui_widget.window));
  gtk_widget_destroyed(ui_widget.window, &ui_widget.window);
}

void hm_ui_window_main_show()
{
  debug(__func__, "INFO", "main window show");
  signal_close_update_main_loop = FALSE;
  for (int i = 0; i < 20; i++)
  {
    memset(lbl_anwr[i], 0x00, sizeof(lbl_anwr[i]));
    memset(lbl_anwr_tmp[i], 0x00, sizeof(lbl_anwr_tmp[i]));
  }
  flag_data_show = 0;
  flag_reset_sensitivity_btn = TRUE;
  flag_popup_show = FALSE;
  g_mutex_lock(&lock_screen_main);
  gettimeofday(&tvLastAccessSTCmdMain, NULL);
  gtk_window_set_keep_above(GTK_WINDOW(ui_widget.window), TRUE);
  gtk_window_set_decorated(GTK_WINDOW(ui_widget.window), FALSE);
  gtk_widget_set_sensitive(ui_widget.window, TRUE);

  gtk_widget_show(ui_widget.window);
  g_mutex_unlock(&lock_screen_main);
  while (gtk_events_pending())
    gtk_main_iteration();
  gdk_threads_add_idle(ui_update_main, NULL);
  pthread_create(&th_main_window, NULL, main_thread, NULL);
}

void hm_ui_window_main_hide()
{
  debug(__func__, "INFO", "main window hide");
  g_mutex_lock(&lock_screen_main);
  gtk_widget_hide(ui_widget.window);
  gtk_widget_set_sensitive(ui_widget.window, FALSE);
  g_mutex_unlock(&lock_screen_main);
  while (gtk_events_pending())
    gtk_main_iteration();
  pthread_join(th_main_window, NULL);
}

gboolean hm_ui_window_main_is_show()
{
  g_mutex_lock(&lock_screen_main);
  gboolean is_window_show = gtk_widget_is_visible(ui_widget.window);
  g_mutex_unlock(&lock_screen_main);
  return is_window_show;
}

int8_t hm_ui_window_main_set_text(int idx_t, char text_t, ...)
{
  g_mutex_lock(&lock_set_text);
  char *text = (char *)calloc(5, sizeof(char));
  if (text == NULL)
  {
    g_mutex_unlock(&lock_set_text);
    return -1;
  }
  // if (strlen(text_t))
  // {
  //   va_list aptr;
  //   va_start(aptr, text_t);
  //   vsnprintf(text, 5, text_t, aptr);
  //   va_end(aptr);
  // }
  switch (idx_t)
  {
  case 1:
    memset(lbl_anwr[0], 0x00, sizeof(lbl_anwr[0]));
    g_usleep(1000);
    lbl_anwr[0][0] = text_t;
    // memcpy(lbl_anwr[0], text, sizeof(lbl_anwr[0]));
    break;
  case 2:
    memset(lbl_anwr[1], 0x00, sizeof(lbl_anwr[0]));
    g_usleep(1000);
    lbl_anwr[1][0] = text_t;
    // memcpy(lbl_anwr[1], text, sizeof(lbl_anwr[0]));
    break;
  case 3:
    memset(lbl_anwr[2], 0x00, sizeof(lbl_anwr[0]));
    g_usleep(1000);
    lbl_anwr[2][0] = text_t;
    // memcpy(lbl_anwr[2], text, sizeof(lbl_anwr[0]));
    break;
  case 4:
    memset(lbl_anwr[3], 0x00, sizeof(lbl_anwr[0]));
    g_usleep(1000);
    lbl_anwr[3][0] = text_t;
    // memcpy(lbl_anwr[3], text, sizeof(lbl_anwr[0]));
    break;
  case 5:
    memset(lbl_anwr[4], 0x00, sizeof(lbl_anwr[0]));
    g_usleep(1000);
    lbl_anwr[4][0] = text_t;
    // memcpy(lbl_anwr[4], text, sizeof(lbl_anwr[0]));
    break;
  case 6:
    memset(lbl_anwr[5], 0x00, sizeof(lbl_anwr[0]));
    g_usleep(1000);
    lbl_anwr[5][0] = text_t;
    // memcpy(lbl_anwr[5], text, sizeof(lbl_anwr[0]));
    break;
  case 7:
    memset(lbl_anwr[6], 0x00, sizeof(lbl_anwr[0]));
    g_usleep(1000);
    lbl_anwr[6][0] = text_t;
    // memcpy(lbl_anwr[6], text, sizeof(lbl_anwr[0]));
    break;
  case 8:
    memset(lbl_anwr[7], 0x00, sizeof(lbl_anwr[0]));
    g_usleep(1000);
    lbl_anwr[7][0] = text_t;
    // memcpy(lbl_anwr[7], text, sizeof(lbl_anwr[0]));
    break;
  case 9:
    memset(lbl_anwr[8], 0x00, sizeof(lbl_anwr[0]));
    g_usleep(1000);
    lbl_anwr[8][0] = text_t;
    // memcpy(lbl_anwr[8], text, sizeof(lbl_anwr[0]));
    break;
  case 10:
    memset(lbl_anwr[9], 0x00, sizeof(lbl_anwr[0]));
    g_usleep(1000);
    lbl_anwr[9][0] = text_t;
    // memcpy(lbl_anwr[9], text, sizeof(lbl_anwr[0]));
    break;
  case 11:
    memset(lbl_anwr[10], 0x00, sizeof(lbl_anwr[0]));
    g_usleep(1000);
    lbl_anwr[10][0] = text_t;
    // memcpy(lbl_anwr[10], text, sizeof(lbl_anwr[0]));
    break;
  case 12:
    memset(lbl_anwr[11], 0x00, sizeof(lbl_anwr[0]));
    g_usleep(1000);
    lbl_anwr[11][0] = text_t;
    // memcpy(lbl_anwr[11], text, sizeof(lbl_anwr[0]));
    break;
  case 13:
    memset(lbl_anwr[12], 0x00, sizeof(lbl_anwr[0]));
    g_usleep(1000);
    lbl_anwr[12][0] = text_t;
    // memcpy(lbl_anwr[12], text, sizeof(lbl_anwr[0]));
    break;
  case 14:
    memset(lbl_anwr[13], 0x00, sizeof(lbl_anwr[0]));
    g_usleep(1000);
    lbl_anwr[13][0] = text_t;
    // memcpy(lbl_anwr[13], text, sizeof(lbl_anwr[0]));
    break;
  case 15:
    memset(lbl_anwr[14], 0x00, sizeof(lbl_anwr[0]));
    g_usleep(1000);
    lbl_anwr[14][0] = text_t;
    // memcpy(lbl_anwr[14], text, sizeof(lbl_anwr[0]));
    break;
  case 16:
    memset(lbl_anwr[15], 0x00, sizeof(lbl_anwr[0]));
    g_usleep(1000);
    lbl_anwr[15][0] = text_t;
    // memcpy(lbl_anwr[15], text, sizeof(lbl_anwr[0]));
    break;
  case 17:
    memset(lbl_anwr[16], 0x00, sizeof(lbl_anwr[0]));
    g_usleep(1000);
    lbl_anwr[16][0] = text_t;
    // memcpy(lbl_anwr[16], text, sizeof(lbl_anwr[0]));
    break;
  case 18:
    memset(lbl_anwr[17], 0x00, sizeof(lbl_anwr[0]));
    g_usleep(1000);
    lbl_anwr[17][0] = text_t;
    // memcpy(lbl_anwr[17], text, sizeof(lbl_anwr[0]));
    break;
  case 19:
    memset(lbl_anwr[18], 0x00, sizeof(lbl_anwr[0]));
    g_usleep(1000);
    lbl_anwr[18][0] = text_t;
    // memcpy(lbl_anwr[18], text, sizeof(lbl_anwr[0]));
    break;
  case 20:
    memset(lbl_anwr[19], 0x00, sizeof(lbl_anwr[0]));
    g_usleep(1000);
    lbl_anwr[19][0] = text_t;
    // memcpy(lbl_anwr[19], text, sizeof(lbl_anwr[0]));
    break;

  default:
    break;
  }

  if (text)
  {
    free(text);
    text = NULL;
  }
  g_mutex_unlock(&lock_set_text);
  return 0;
}

static void hm_ui_window_main_get_object()
{
  hm_gui_get_object_helper(&ui_widget.window, "main_ui_window");
  hm_gui_get_object_helper(&ui_widget.container, "main_ui_main_container");
  hm_gui_get_object_helper(&ui_widget.animation_box, "main_ui_animation_box");
  hm_gui_get_object_helper(&ui_widget.animation, "main_ui_animation");
  hm_gui_get_object_helper(&ui_widget.back_to_menu_button, "main_ui_back_btn");
  hm_gui_get_object_helper(&ui_widget.back_to_menu_box, "main_ui_back_box");
  hm_gui_get_object_helper(&ui_widget.answer_box, "main_ui_answer_box");
  hm_gui_get_object_helper(&ui_widget.keyboard_box, "main_ui_keyboard_box");
  hm_gui_get_object_helper(&ui_widget.anws_label_colomn[0], "answer_colomn_1");
  hm_gui_get_object_helper(&ui_widget.anws_label_colomn[1], "answer_colomn_2");
  hm_gui_get_object_helper(&ui_widget.anws_label_colomn[2], "answer_colomn_3");
  hm_gui_get_object_helper(&ui_widget.anws_label_colomn[3], "answer_colomn_4");
  hm_gui_get_object_helper(&ui_widget.anws_label_colomn[4], "answer_colomn_5");
  hm_gui_get_object_helper(&ui_widget.anws_label_colomn[5], "answer_colomn_6");
  hm_gui_get_object_helper(&ui_widget.anws_label_colomn[6], "answer_colomn_7");
  hm_gui_get_object_helper(&ui_widget.anws_label_colomn[7], "answer_colomn_8");
  hm_gui_get_object_helper(&ui_widget.anws_label_colomn[8], "answer_colomn_9");
  hm_gui_get_object_helper(&ui_widget.anws_label_colomn[9], "answer_colomn_10");
  hm_gui_get_object_helper(&ui_widget.anws_label_colomn[10], "answer_colomn_11");
  hm_gui_get_object_helper(&ui_widget.anws_label_colomn[11], "answer_colomn_12");
  hm_gui_get_object_helper(&ui_widget.anws_label_colomn[12], "answer_colomn_13");
  hm_gui_get_object_helper(&ui_widget.anws_label_colomn[13], "answer_colomn_14");
  hm_gui_get_object_helper(&ui_widget.anws_label_colomn[14], "answer_colomn_15");
  hm_gui_get_object_helper(&ui_widget.anws_label_colomn[15], "answer_colomn_16");
  hm_gui_get_object_helper(&ui_widget.anws_label_colomn[16], "answer_colomn_17");
  hm_gui_get_object_helper(&ui_widget.anws_label_colomn[17], "answer_colomn_18");
  hm_gui_get_object_helper(&ui_widget.anws_label_colomn[18], "answer_colomn_19");
  hm_gui_get_object_helper(&ui_widget.anws_label_colomn[19], "answer_colomn_20");

  hm_gui_get_object_helper(&ui_widget.btn_[0], "window_keyboard_button_key_a");
  hm_gui_get_object_helper(&ui_widget.btn_[1], "window_keyboard_button_key_b");
  hm_gui_get_object_helper(&ui_widget.btn_[2], "window_keyboard_button_key_c");
  hm_gui_get_object_helper(&ui_widget.btn_[3], "window_keyboard_button_key_d");
  hm_gui_get_object_helper(&ui_widget.btn_[4], "window_keyboard_button_key_e");
  hm_gui_get_object_helper(&ui_widget.btn_[5], "window_keyboard_button_key_f");
  hm_gui_get_object_helper(&ui_widget.btn_[6], "window_keyboard_button_key_g");
  hm_gui_get_object_helper(&ui_widget.btn_[7], "window_keyboard_button_key_h");
  hm_gui_get_object_helper(&ui_widget.btn_[8], "window_keyboard_button_key_i");
  hm_gui_get_object_helper(&ui_widget.btn_[9], "window_keyboard_button_key_j");
  hm_gui_get_object_helper(&ui_widget.btn_[10], "window_keyboard_button_key_k");
  hm_gui_get_object_helper(&ui_widget.btn_[11], "window_keyboard_button_key_l");
  hm_gui_get_object_helper(&ui_widget.btn_[12], "window_keyboard_button_key_m");
  hm_gui_get_object_helper(&ui_widget.btn_[13], "window_keyboard_button_key_n");
  hm_gui_get_object_helper(&ui_widget.btn_[14], "window_keyboard_button_key_o");
  hm_gui_get_object_helper(&ui_widget.btn_[15], "window_keyboard_button_key_p");
  hm_gui_get_object_helper(&ui_widget.btn_[16], "window_keyboard_button_key_q");
  hm_gui_get_object_helper(&ui_widget.btn_[17], "window_keyboard_button_key_r");
  hm_gui_get_object_helper(&ui_widget.btn_[18], "window_keyboard_button_key_s");
  hm_gui_get_object_helper(&ui_widget.btn_[19], "window_keyboard_button_key_t");
  hm_gui_get_object_helper(&ui_widget.btn_[20], "window_keyboard_button_key_u");
  hm_gui_get_object_helper(&ui_widget.btn_[21], "window_keyboard_button_key_v");
  hm_gui_get_object_helper(&ui_widget.btn_[22], "window_keyboard_button_key_w");
  hm_gui_get_object_helper(&ui_widget.btn_[23], "window_keyboard_button_key_x");
  hm_gui_get_object_helper(&ui_widget.btn_[24], "window_keyboard_button_key_y");
  hm_gui_get_object_helper(&ui_widget.btn_[25], "window_keyboard_button_key_z");

  gtk_widget_set_name(ui_widget.window, "main_ui_window");
  gtk_widget_set_name(ui_widget.container, "main_ui_main_container");
  gtk_widget_set_name(ui_widget.answer_box, "main_ui_answer_box");
  gtk_widget_set_name(ui_widget.back_to_menu_box, "main_ui_back_box");
  gtk_widget_set_name(ui_widget.back_to_menu_button, "main_ui_back_btn");
  gtk_widget_set_name(ui_widget.animation_box, "main_ui_animation_box");
  gtk_widget_set_name(ui_widget.animation, "main_ui_animation");
  gtk_widget_set_name(ui_widget.keyboard_box, "main_ui_keyboard_box");

  gtk_window_set_position(GTK_WINDOW(ui_widget.window), GTK_WIN_POS_CENTER_ALWAYS);
  gtk_window_fullscreen(GTK_WINDOW(ui_widget.window));
}
static void hm_ui_window_main_signal_connect()
{
  g_signal_connect(G_OBJECT(ui_widget.back_to_menu_button), "clicked", G_CALLBACK(on_window_main_signal), GINT_TO_POINTER(1));

  for (int i = 0; i < 26; i++)
    g_signal_connect(G_OBJECT(ui_widget.btn_[i]), "clicked", G_CALLBACK(on_window_main_signal), GINT_TO_POINTER(i + 97));

  g_signal_connect(G_OBJECT(ui_widget.window), "key_press_event", G_CALLBACK(hm_ui_window_main_keypress), NULL);
}

void on_window_main_signal(GtkWidget *widget, gpointer t_data)
{
  int data = GPOINTER_TO_INT(t_data);
  debug(__func__, "INFO", "on signal main trigger %i", data);
  struct timeval tvNow;
  long long difftm = 0;
  gettimeofday(&tvNow, NULL);
  difftm = ((tvNow.tv_sec - tvLastAccessSTCmdMain.tv_sec) * 1000 + (tvNow.tv_usec - tvLastAccessSTCmdMain.tv_usec) / 1000);
  switch (data)
  {
  case 1:
    debug(__func__, "INFO", "case 1");
    if (signal_main_flag && difftm > 500)
    {
      debug(__func__, "INFO", "open menu window");
      signal_main_flag = FALSE;
      signal_close_update_main_loop = TRUE;
      hm_ui_window_menu_show();
      time_t t_prev_wait, t_wait = 0;
      time(&t_prev_wait);
      int8_t open_menu_flag = 1;
      while (!hm_ui_window_menu_is_show())
      {
        time(&t_wait);
        if (t_wait - t_prev_wait >= 5)
        {
          hm_ui_window_menu_hide();
          open_menu_flag = 0;
          break;
        }
      }
      if (open_menu_flag)
      {
        debug(__func__, "INFO", "hide main window");
        hm_ui_window_main_hide();
      }
    }
    gettimeofday(&tvLastAccessSTCmdMain, NULL);
    signal_main_flag = TRUE;
    debug(__func__, "INFO", "done");
    break;

  case 2:
    break;

  default:
    if (data >= 97 && data <= 122)
    {
      flag_data_show = data;
      gtk_widget_set_sensitive(ui_widget.btn_[data - 97], FALSE);
    }
    break;
  }
}

gboolean ui_update_main(gpointer not_used)
{
  if (signal_close_update_main_loop)
    return FALSE;

  for (int i = 0; i < 20; i++)
  {
    if (strcmp(lbl_anwr[i], lbl_anwr_tmp[i]) != 0)
    {
      g_mutex_lock(&lock_set_text);
      // if (strcmp(lbl_anwr[i], " ") == 0)
      // {
      //   gtk_widget_hide(ui_widget.anws_label_colomn[i]);
      // }
      // else
      // {
      gtk_widget_show(ui_widget.anws_label_colomn[i]);
      g_usleep(1000);
      gtk_label_set_text((GtkLabel *)ui_widget.anws_label_colomn[i], lbl_anwr[i]);
      // }
      memset(lbl_anwr_tmp[i], 0x00, sizeof(lbl_anwr_tmp[i]));
      strcpy(lbl_anwr_tmp[i], lbl_anwr[i]);
      g_mutex_unlock(&lock_set_text);
      while (gtk_events_pending())
        gtk_main_iteration();
    }
  }

  if (flag_reset_sensitivity_btn == TRUE)
  {
    for (int i = 0; i < 26; i++)
    {
      gtk_widget_set_sensitive(ui_widget.btn_[i], TRUE);
    }
    flag_reset_sensitivity_btn = FALSE;
  }

  if (main_img_state == WAIT_FOR_SHOW_IMG)
  {
  }
  else if (main_img_state == START_IMG)
  {
    hm_gui_load_image_helper(&ui_widget.animation, 600, 900, "asset/img/Slide1.PNG");
    g_usleep(1000000);
    hm_gui_load_image_helper(&ui_widget.animation, 600, 900, "asset/img/Slide2.PNG");
    g_usleep(1000000);
    hm_gui_load_image_helper(&ui_widget.animation, 600, 900, "asset/img/Slide3.PNG");
    main_img_state = WAIT_FOR_SHOW_IMG;
  }
  else if (main_img_state == SIX_REMAIN_IMG)
  {
    hm_gui_load_image_helper(&ui_widget.animation, 600, 900, "asset/img/Slide4.PNG");
    main_img_state = WAIT_FOR_SHOW_IMG;
  }
  else if (main_img_state == FIVE_REMAIN_IMG)
  {
    hm_gui_load_image_helper(&ui_widget.animation, 600, 900, "asset/img/Slide5.PNG");
    main_img_state = WAIT_FOR_SHOW_IMG;
  }
  else if (main_img_state == FOUR_REMAIN_IMG)
  {
    hm_gui_load_image_helper(&ui_widget.animation, 600, 900, "asset/img/Slide6.PNG");
    main_img_state = WAIT_FOR_SHOW_IMG;
  }
  else if (main_img_state == THREE_REMAIN_IMG)
  {
    hm_gui_load_image_helper(&ui_widget.animation, 600, 900, "asset/img/Slide7.PNG");
    main_img_state = WAIT_FOR_SHOW_IMG;
  }
  else if (main_img_state == TWO_REMAIN_IMG)
  {
    hm_gui_load_image_helper(&ui_widget.animation, 600, 900, "asset/img/Slide8.PNG");
    main_img_state = WAIT_FOR_SHOW_IMG;
  }
  else if (main_img_state == ONE_REMAIN_IMG)
  {
    hm_gui_load_image_helper(&ui_widget.animation, 600, 900, "asset/img/Slide9.PNG");
    main_img_state = WAIT_FOR_SHOW_IMG;
  }
  else if (main_img_state == LOSE_IMG)
  {
    hm_gui_load_image_helper(&ui_widget.animation, 600, 900, "asset/img/Slide10.PNG");
    main_img_state = WAIT_FOR_SHOW_IMG;
  }
  else if (main_img_state == WIN_IMG)
  {
    hm_gui_load_image_helper(&ui_widget.animation, 600, 900, "asset/img/Slide11.PNG");
    g_usleep(1000000);
    hm_gui_load_image_helper(&ui_widget.animation, 600, 900, "asset/img/Slide12.PNG");
    g_usleep(1000000);
    hm_gui_load_image_helper(&ui_widget.animation, 600, 900, "asset/img/Slide13.PNG");
    main_img_state = WAIT_FOR_SHOW_IMG;
  }

  if (flag_popup_show == TRUE)
  {
    hm_ui_window_popup_show(ui_widget.window);
    flag_popup_show = FALSE;
  }

  while (gtk_events_pending())
    gtk_main_iteration();
  g_usleep(1000);
  return TRUE;
}

void *main_thread(void *ptr)
{
  typedef struct node node_t;
  node_t *head = NULL;
  node_t *tmp_node = NULL;
  int node_select = 0;

  hm_conf_get_words(FILE_WORDS, &head);

  char buff_show[20];

  while (!signal_close_update_main_loop)
  {
    if (main_state_thread == START_GAME)
    {
      srand(time(0));
      node_select = hm_lilist_count_node(head);
      debug(__func__, "INFO", "jumlah node %d", node_select);
      node_select = rand() % node_select + 1;

      tmp_node = NULL;

      tmp_node = hm_lilist_find_node_with_id(head, node_select);
      debug(__func__, "INFO", "Node select is %s", tmp_node->value);

      memset(buff_show, 0x00, sizeof(buff_show));
      hm_tools_centering_word(tmp_node->value, buff_show, 1);
      main_state_thread = SHOW_DATA;
      main_img_state = START_IMG;
      lives = 7;
      main_img_state = lives;
    }

    else if (main_state_thread == SHOW_DATA)
    {
      for (int show = 0; show < 20; show++)
      {
        hm_ui_window_main_set_text(show + 1, buff_show[show]);
      }
      main_state_thread = CHECK_WON_OR_NOT;
    }

    else if (main_state_thread == CHECK_WON_OR_NOT)
    {
      if (lives == 0)
        main_state_thread = FAIL;

      else if (hm_tools_check_won_game(buff_show))
        main_state_thread = GUESS_DATA;
      else
        main_state_thread = SUCCESS;

      if (lives != 7 && main_state_thread != SUCCESS)
        main_img_state = lives;
    }
    else if (main_state_thread == GUESS_DATA)
    {
      if (flag_data_show)
      {
        lives += hm_tools_guessing_word(buff_show, tmp_node->value, flag_data_show);
        flag_data_show = 0;
        main_state_thread = SHOW_DATA;
      }
    }
    else if (main_state_thread == SUCCESS)
    {
      debug(__func__, "INFO", "Congrats, U Won The Game");
      hm_ui_window_main_won_game();
      main_img_state = WIN_IMG;
      main_state_thread = WAIT_FOR_CHANGE_STATE;
      usleep(5000000);
      flag_popup_show = TRUE;
    }

    else if (main_state_thread == FAIL)
    {
      debug(__func__, "INFO", "U suck");
      hm_ui_window_main_lost_game();
      main_img_state = LOSE_IMG;
      main_state_thread = WAIT_FOR_CHANGE_STATE;
      usleep(5000000);
      flag_popup_show = TRUE;
    }
    usleep(1000);
  }
  hm_lilist_delete_node(&head);
  pthread_exit(NULL);
}

static gboolean hm_ui_window_main_keypress(GtkWidget *_widget_, GdkEventKey *_event_, gpointer _data_)
{
  switch (_event_->keyval)
  {
  default:
    if (_event_->keyval >= 97 && _event_->keyval <= 122)
    {
      flag_data_show = _event_->keyval;
      gtk_widget_set_sensitive(ui_widget.btn_[_event_->keyval - 97], FALSE);
    }
    break;
  }
  return FALSE;
}

void hm_ui_window_main_won_game()
{
  char buff_show[20];
  memset(buff_show, 0x00, sizeof(buff_show));
  hm_tools_centering_word("u won, congrats", buff_show, 0);
  for (int show = 0; show < 20; show++)
  {
    hm_ui_window_main_set_text(show + 1, buff_show[show]);
  }
}
void hm_ui_window_main_lost_game()
{
  char buff_show[20];
  memset(buff_show, 0x00, sizeof(buff_show));
  hm_tools_centering_word("u lost, moron", buff_show, 0);
  for (int show = 0; show < 20; show++)
  {
    hm_ui_window_main_set_text(show + 1, buff_show[show]);
  }
}

void hm_ui_window_main_sestive(gboolean flag)
{
  if (flag == TRUE)
    gtk_widget_set_sensitive(ui_widget.window, TRUE);
  else
    gtk_widget_set_sensitive(ui_widget.window, FALSE);
}

void hm_ui_window_main_start_game()
{
  printf("chnage to start game\n");
  main_state_thread = START_GAME;
  flag_reset_sensitivity_btn = TRUE;
}