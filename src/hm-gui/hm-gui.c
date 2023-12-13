
#include "hm-gui.h"
#include "hm-debug/hm-debug.h"
#include "hm-gui-menu.h"
#include "hm-gui-main.h"
#include "hm-gui-popup.h"
#include "hm-tools/hm-tools.h"

#define GLADE_FILE "asset/ui_hangman.glade"
#define CSS_FILE "asset/style.css"

pthread_t th_ui, th_ui_exit;
GtkBuilder *builder;
gboolean flag_ui_exit = FALSE;
pthread_mutex_t lock_gui = PTHREAD_MUTEX_INITIALIZER;

void gui_start(int argc, char **argv);
void hm_gui_gtk_init();

/**
 * TODO:
 * [] Complete the gui_start function
 */
void *th_gui(void *arg)
{
  pthread_detach(pthread_self());
  gui_start(0, NULL);
  pthread_exit(NULL);
}

int hm_gui_start()
{
  int err = 0;
  if ((err = pthread_create(&th_ui, NULL, th_gui, NULL)) != 0)
  {
    return -1;
  }
  return 0;
}

/**
 * TODO:
 * [] Complete hm_gui_gtk_init
 * [] Complete hm_ui_window_menu_show
 */
void gui_start(int argc, char **argv)
{
  gtk_init(&argc, &argv);
  hm_gui_gtk_init();
  hm_ui_window_menu_show();
  gtk_main();
  g_usleep(100000);
  /* when gui exit, execute this */
  while (!flag_ui_exit)
  {
    g_usleep(1000);
  }
  hm_tools_set_app_stop();
  debug(__func__, "INFO", "GUI Closed");
}

void *gui_exit_thread(void *arg)
{
  pthread_detach(pthread_self());
  hm_ui_window_menu_deinitialize();
  hm_ui_window_main_deinitialize();
  hm_ui_window_popup_deinitialize();
  g_usleep(100000);
  gtk_main_quit();
  flag_ui_exit = TRUE;
  pthread_exit(NULL);
}

int gui_exit()
{
  int err = 0;
  if ((err = pthread_create(&th_ui_exit, NULL, gui_exit_thread, NULL)) != 0)
  {
    return -1;
  }
  return 0;
}

void hm_gui_gtk_init()
{
  GError *err = NULL;
  builder = gtk_builder_new();
  if (!gtk_builder_add_from_file(builder, GLADE_FILE, &err))
  {
    debug(__func__, "ERROR", "FAILED GET %s", GLADE_FILE);
    g_print("[%d] %s\n", err->code, err->message);
    g_error_free(err);
    exit(EXIT_FAILURE);
  }
  else
    debug(__func__, "INFO", "BUILDER FROM %s", GLADE_FILE);

  g_clear_error(&err);

  GtkCssProvider *cssProvider;
  cssProvider = gtk_css_provider_new();
  gtk_css_provider_load_from_path(cssProvider, CSS_FILE, &err);
  if (err)
  {
    g_print("[%d] %s\n", err->code, err->message);
    g_error_free(err);
    exit(EXIT_FAILURE);
  }
  gtk_builder_connect_signals(builder, NULL);
  hm_ui_window_main_initialize(); // belum
  hm_ui_window_menu_initialize();
  hm_ui_window_popup_initialize();
  gtk_style_context_add_provider_for_screen(gdk_screen_get_default(), GTK_STYLE_PROVIDER(cssProvider), GTK_STYLE_PROVIDER_PRIORITY_USER);
  g_object_unref(builder);
}

/**
 * @brief inform when failed to get widget object from builder
 * @param widget_t pointer widget yang ingin dihubungukan dengan object di glade
 * @param widget_name_t id object yang diberikan di glade
 */

gboolean hm_gui_get_object_helper(GtkWidget **widget_t, gchar *widget_name_t, ...)
{
  if (widget_name_t == NULL || strlen((char *)widget_name_t) == 0)
  {
    debug(__func__, "ERROR", "WIDGET NAME NOT VALID ! (null)");
    return TRUE;
  }
  gchar *widget_name = NULL;
  va_list aptr;
  va_start(aptr, widget_name_t);
  widget_name = g_strdup_vprintf(widget_name_t, aptr);
  va_end(aptr);

  if (widget_name == NULL)
    return TRUE;

  *widget_t = GTK_WIDGET(gtk_builder_get_object(builder, widget_name));
  if (widget_name == NULL)
  {
    debug(__func__, "ERROR", "FAILED GET %s", widget_name);
    g_free(widget_name);
    widget_name = NULL;
    return TRUE;
  }
  g_free(widget_name);
  widget_name = NULL;
  return FALSE;
}

gboolean hm_gui_load_image_helper(GtkWidget **_widget_, int _width_, int _height_, gchar *_filename_, ...)
{
  pthread_mutex_lock(&lock_gui);
  if (_filename_ == NULL)
  {
    debug(__func__, "ERROR:", "FILENAME NOT VALID ! (null)");
    return TRUE;
  }

  gchar *filename = NULL;
  va_list aptr;
  va_start(aptr, _filename_);
  filename = g_strdup_vprintf(_filename_, aptr);
  va_end(aptr);
  if (filename == NULL)
  {
    return TRUE;
  }
  GdkPixbuf *img_loader = NULL;
  GdkPixbuf *img = NULL;
  GError *err = NULL;
  img_loader = gdk_pixbuf_new_from_file(filename, &err);
  if (!img_loader)
  {
    if (err)
    {
      if (err->code == 2)
        debug(__func__, "ERROR:", "[%d] FAILED TO LOAD IMAGE %s", err->code, filename);
      else
        g_print("[%d] %s\n", err->code, err->message);
      g_error_free(err);
    }
    g_free(filename);
    filename = NULL;
    return TRUE;
  }

  GdkRectangle workarea = {0};
  gdk_monitor_get_workarea(
      gdk_display_get_primary_monitor(gdk_display_get_default()),
      &workarea);

  gint img_width = abs((int)(workarea.width / ((double)1920 / _width_)));
  gint img_height = abs((int)(workarea.height / ((double)1080 / _height_)));

  img = gdk_pixbuf_scale_simple(img_loader, abs(img_width), abs(img_height), GDK_INTERP_BILINEAR);
  if (!img)
  {
    g_free(filename);
    filename = NULL;
    return TRUE;
  }
  gtk_image_set_from_pixbuf((GtkImage *)*_widget_, img);
  g_object_unref(img);
  g_object_unref(img_loader);
  g_free(filename);
  filename = NULL;
  pthread_mutex_unlock(&lock_gui);
  while (gtk_events_pending())
    gtk_main_iteration();
  return FALSE;
}