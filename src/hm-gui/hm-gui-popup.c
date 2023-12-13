#include "hm-gui-popup.h"
#include "hm-debug/hm-debug.h"
#include "hm-gui-main.h"

typedef struct
{
  GtkWidget *window;
  GtkWidget *container;
  GtkWidget *lbl_title;
  GtkWidget *btn_yes;
  GtkWidget *btn_no;
  GtkWidget *btn_yes_icon;
  GtkWidget *btn_no_icon;
} widget_popup_window_t;

static widget_popup_window_t ui_widget;
static GMutex lock_button;
static GMutex lock_screen_popup;

gboolean close_popup_window_thread(gpointer not_used)
{
  g_usleep(10000);
  hm_ui_window_popup_hide(); // hide popup window
  debug(__func__, "INFO", "done close popup");
  return FALSE;
}

static void on_window_popup_signal(GtkWidget *widget, gpointer _data_)
{
  int data = GPOINTER_TO_INT(_data_);
  switch (data)
  {
  case 0: //  play again
    g_mutex_lock(&lock_button);
    g_usleep(10000);
    gdk_threads_add_idle(close_popup_window_thread, NULL);
    hm_ui_window_main_start_game();
    g_mutex_unlock(&lock_button);

    break;

  case 1: // not play again
    g_mutex_lock(&lock_button);
    g_usleep(10000);
    gdk_threads_add_idle(close_popup_window_thread, NULL);
    g_mutex_unlock(&lock_button);
    on_window_main_signal(NULL, GINT_TO_POINTER(1));

    break;

  default:
    break;
  }
}

static void ui_window_popup_signal_connect()
{
  g_signal_connect(G_OBJECT(ui_widget.btn_yes), "clicked", G_CALLBACK(on_window_popup_signal), GINT_TO_POINTER(0));
  g_signal_connect(G_OBJECT(ui_widget.btn_no), "clicked", G_CALLBACK(on_window_popup_signal), GINT_TO_POINTER(1));
}

static void ui_window_popup_get_object()
{
  hm_gui_get_object_helper(&ui_widget.window, "popup_ui_window");
  hm_gui_get_object_helper(&ui_widget.lbl_title, "popup_window_title");
  hm_gui_get_object_helper(&ui_widget.container, "popup_window_container");

  hm_gui_get_object_helper(&ui_widget.btn_yes, "popup_window_yes_btn");
  hm_gui_get_object_helper(&ui_widget.btn_yes_icon, "yes_icon");
  hm_gui_get_object_helper(&ui_widget.btn_no, "popup_window_no_btn");
  hm_gui_get_object_helper(&ui_widget.btn_no_icon, "no_icon");

  // CSS disamakan dengan window tap kartu
  gtk_widget_set_name(ui_widget.window, "popup_ui_window");
  gtk_widget_set_name(ui_widget.btn_yes_icon, "yes_icon");
  gtk_widget_set_name(ui_widget.btn_no_icon, "no_icon");
  gtk_widget_set_name(ui_widget.btn_yes, "popup_window_yes_btn");
  gtk_widget_set_name(ui_widget.btn_no, "popup_window_no_btn");
  gtk_widget_set_name(ui_widget.container, "popup_window_container");
  gtk_window_set_position(GTK_WINDOW(ui_widget.window), GTK_WIN_POS_CENTER_ALWAYS);
}

void hm_ui_window_popup_initialize()
{
  g_mutex_init(&lock_button);
  g_mutex_init(&lock_screen_popup);
  ui_window_popup_get_object();
  ui_window_popup_signal_connect();
  gtk_window_set_destroy_with_parent(GTK_WINDOW(ui_widget.window), TRUE);
}

void hm_ui_window_popup_deinitialize()
{
  g_mutex_clear(&lock_button);
  g_mutex_clear(&lock_screen_popup);
  gtk_window_close(GTK_WINDOW(ui_widget.window));
  gtk_widget_destroyed(ui_widget.window, &ui_widget.window);
}

void hm_ui_window_popup_show(GtkWidget *_clientWindow_)
{
  debug(__func__, "INFO", "popup window show");
  g_mutex_lock(&lock_screen_popup);
  hm_ui_window_main_sestive(FALSE);
  gtk_window_set_keep_above(GTK_WINDOW(_clientWindow_), FALSE);
  gtk_window_set_keep_above(GTK_WINDOW(ui_widget.window), TRUE);
  gtk_window_set_decorated(GTK_WINDOW(ui_widget.window), FALSE);
  gtk_widget_show(ui_widget.window);
  gtk_widget_set_sensitive(ui_widget.window, TRUE);
  g_mutex_unlock(&lock_screen_popup);
  while (gtk_events_pending())
    gtk_main_iteration();
}

void hm_ui_window_popup_hide()
{
  debug(__func__, "INFO", "popup window hide");
  g_mutex_lock(&lock_screen_popup);
  hm_ui_window_main_sestive(TRUE);
  gtk_widget_hide(ui_widget.window);
  gtk_widget_set_sensitive(ui_widget.window, FALSE);
  g_mutex_unlock(&lock_screen_popup);
  while (gtk_events_pending())
    gtk_main_iteration();
}
