#include <gtk/gtk.h>
#include <glib.h>
#include <stdlib.h>
#include <sys/time.h>
#include <pthread.h>

#include "hm-gui-menu.h"
#include "hm-gui-main.h"
#include "hm-debug/hm-debug.h"
#include "hm-tools/hm-tools.h"

typedef struct
{
  GtkWidget *window;
  GtkWidget *container;
  GtkWidget *container_border;
  GtkWidget *play_btn;
  GtkWidget *exit_btn;
  GtkWidget *mute_btn;
  GtkWidget *mute_btn_img;
} widget_window_menu_t;

static widget_window_menu_t ui_widget;
GMutex lock_screen_menu;
struct timeval tvLastAccessSTCmdMenu = {0, 0};
gboolean signal_menu_flag = TRUE;
gboolean signal_close_update_loop = FALSE;
gboolean flag_mute_sound = FALSE;

pthread_t th_open_main;

static void hm_ui_window_menu_get_object();
static void hm_ui_window_menu_signal_connect();
void on_window_menu_signal(GtkWidget *widget, gpointer _data_);
void stop_music();

void hm_ui_window_menu_initialize()
{
  g_mutex_init(&lock_screen_menu);
  hm_ui_window_menu_get_object();
  hm_ui_window_menu_signal_connect();
  gtk_window_set_destroy_with_parent(GTK_WINDOW(ui_widget.window), TRUE);
}

void hm_ui_window_menu_deinitialize()
{
  signal_close_update_loop = TRUE;
  stop_music();
  g_mutex_clear(&lock_screen_menu);
  gtk_window_close(GTK_WINDOW(ui_widget.window));
  gtk_widget_destroyed(ui_widget.window, &ui_widget.window);
}

void play_music()
{
  system("pkill -f audacious");
  system("audacious -p asset/sound/backsound-menu.mp3 -H &");
}

void pause_play_music()
{
  system("audacious -t");
}

void stop_music()
{
  system("pkill -f audacious");
}

void hm_ui_window_menu_show()
{
  debug(__func__, "INFO", "window menu show");
  signal_close_update_loop = FALSE;
  flag_mute_sound = FALSE;
  play_music();
  hm_gui_load_image_helper(&ui_widget.mute_btn_img, 100, 100, "asset/img/sound_on.png");
  g_mutex_lock(&lock_screen_menu);
  gettimeofday(&tvLastAccessSTCmdMenu, NULL);
  gtk_window_set_keep_above(GTK_WINDOW(ui_widget.window), TRUE);
  gtk_window_set_decorated(GTK_WINDOW(ui_widget.window), FALSE);
  gtk_widget_set_sensitive(ui_widget.window, TRUE);
  gtk_widget_show(ui_widget.window);
  g_mutex_unlock(&lock_screen_menu);
  while (gtk_events_pending())
    gtk_main_iteration();
}

void hm_ui_window_menu_hide()
{
  debug(__func__, "INFO", "window menu hide");
  signal_close_update_loop = TRUE;
  stop_music();
  g_mutex_lock(&lock_screen_menu);
  gtk_widget_hide(ui_widget.window);
  gtk_widget_set_sensitive(ui_widget.window, FALSE);
  g_mutex_unlock(&lock_screen_menu);
  while (gtk_events_pending())
    gtk_main_iteration();
}

gboolean hm_ui_window_menu_is_show()
{
  g_mutex_lock(&lock_screen_menu);
  gboolean is_window_show = gtk_widget_is_visible(ui_widget.window);
  g_mutex_unlock(&lock_screen_menu);
  return is_window_show;
}

static void hm_ui_window_menu_get_object()
{
  hm_gui_get_object_helper(&ui_widget.window, "menu_ui_window");
  hm_gui_get_object_helper(&ui_widget.container, "menu_ui_main_container");
  hm_gui_get_object_helper(&ui_widget.container_border, "menu_ui_main_container_border");
  hm_gui_get_object_helper(&ui_widget.play_btn, "menu_ui_play_btn");
  hm_gui_get_object_helper(&ui_widget.exit_btn, "menu_ui_exit_btn");
  hm_gui_get_object_helper(&ui_widget.mute_btn_img, "menu_ui_mute_btn_img");
  hm_gui_get_object_helper(&ui_widget.mute_btn, "menu_ui_mute_btn");

  gtk_widget_set_name(ui_widget.window, "menu_ui_window");
  gtk_widget_set_name(ui_widget.container, "menu_ui_main_container");
  gtk_widget_set_name(ui_widget.container_border, "menu_ui_main_container_border");
  gtk_widget_set_name(ui_widget.play_btn, "menu_ui_play_btn");
  gtk_widget_set_name(ui_widget.exit_btn, "menu_ui_play_btn");
  gtk_widget_set_name(ui_widget.mute_btn, "menu_ui_mute_btn");
  gtk_widget_set_name(ui_widget.mute_btn_img, "menu_ui_mute_btn_img");

  gtk_window_set_position(GTK_WINDOW(ui_widget.window), GTK_WIN_POS_CENTER_ALWAYS);
  gtk_window_fullscreen(GTK_WINDOW(ui_widget.window));
}

static void hm_ui_window_menu_signal_connect()
{
  g_signal_connect(G_OBJECT(ui_widget.play_btn), "clicked", G_CALLBACK(on_window_menu_signal), GINT_TO_POINTER(1));
  g_signal_connect(G_OBJECT(ui_widget.exit_btn), "clicked", G_CALLBACK(on_window_menu_signal), GINT_TO_POINTER(2));
  g_signal_connect(G_OBJECT(ui_widget.mute_btn), "clicked", G_CALLBACK(on_window_menu_signal), GINT_TO_POINTER(3));
}

void *gui_open_win_main_thread(void *arg)
{
  pthread_detach(pthread_self());

  debug(__func__, "INFO", "open main window");
  signal_menu_flag = FALSE;
  hm_ui_window_main_show();
  time_t t_prev_wait, t_wait = 0;
  time(&t_prev_wait);
  int8_t open_main_flag = 1;
  while (!hm_ui_window_main_is_show())
  {
    time(&t_wait);
    if (t_wait - t_prev_wait >= 5)
    {
      hm_ui_window_main_hide();
      open_main_flag = 0;
      break;
    }
  }
  if (open_main_flag)
  {
    hm_ui_window_menu_hide();
  }
  gettimeofday(&tvLastAccessSTCmdMenu, NULL);
  signal_menu_flag = TRUE;
  debug(__func__, "INFO", "done menu");

  pthread_exit(NULL);
}

int gui_open_win_main()
{
  int err = 0;
  if ((err = pthread_create(&th_open_main, NULL, gui_open_win_main_thread, NULL)) != 0)
  {
    return -1;
  }
  return 0;
}

void on_window_menu_signal(GtkWidget *widget, gpointer _data_)
{
  int data = GPOINTER_TO_INT(_data_);
  debug(__func__, "INFO", "on signal menu trigger %i", data);
  struct timeval tvNow;
  long long difftm = 0;
  switch (data)
  {
  case 1:
    // debug(__func__, "INFO", "case 1");
    // gettimeofday(&tvNow, NULL);
    // difftm = ((tvNow.tv_sec - tvLastAccessSTCmdMenu.tv_sec) * 1000 + (tvNow.tv_usec - tvLastAccessSTCmdMenu.tv_usec) / 1000);
    // if (signal_menu_flag && difftm > 500)
    // {
    //   gui_open_win_main();
    // }
    debug(__func__, "INFO", "case 1");
    gettimeofday(&tvNow, NULL);
    difftm = ((tvNow.tv_sec - tvLastAccessSTCmdMenu.tv_sec) * 1000 + (tvNow.tv_usec - tvLastAccessSTCmdMenu.tv_usec) / 1000);
    if (signal_menu_flag && difftm > 500)
    {
      debug(__func__, "INFO", "open main window");
      signal_menu_flag = FALSE;
      hm_ui_window_main_show();
      time_t t_prev_wait, t_wait = 0;
      time(&t_prev_wait);
      int8_t open_main_flag = 1;
      while (!hm_ui_window_main_is_show())
      {
        time(&t_wait);
        if (t_wait - t_prev_wait >= 5)
        {
          hm_ui_window_main_hide();
          open_main_flag = 0;
          break;
        }
      }
      if (open_main_flag)
      {
        hm_ui_window_menu_hide();
      }
    }
    gettimeofday(&tvLastAccessSTCmdMenu, NULL);
    signal_menu_flag = TRUE;
    debug(__func__, "INFO", "done menu");

    break;

  case 2:
    gui_exit();
    break;

  case 3:
    if (flag_mute_sound == FALSE)
    {
      flag_mute_sound = TRUE;
      hm_gui_load_image_helper(&ui_widget.mute_btn_img, 100, 100, "asset/img/sound_off.png");
      gtk_widget_set_name(ui_widget.mute_btn, "menu_ui_unmute_btn");
      pause_play_music();
    }
    else
    {
      flag_mute_sound = FALSE;
      hm_gui_load_image_helper(&ui_widget.mute_btn_img, 100, 100, "asset/img/sound_on.png");
      gtk_widget_set_name(ui_widget.mute_btn, "menu_ui_mute_btn");
      pause_play_music();
    }
    break;
  }
}
