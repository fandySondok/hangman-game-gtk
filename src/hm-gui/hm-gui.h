#ifndef HM_GUI_H
#define HM_GUI_H

#include <pthread.h>
#include <gtk/gtk.h>
#include <glib.h>
#include <stdlib.h>
#include <gdk/gdkkeysyms-compat.h>
#include <gdk/gdkscreen.h>

gboolean hm_gui_get_object_helper(GtkWidget **widget_t, gchar *widget_name_t, ...);
int hm_gui_start();
int gui_exit();
gboolean hm_gui_load_image_helper(GtkWidget **_widget_, int _width_, int _height_, gchar *_filename_, ...);
// gboolean hm_gui_load_image_helper(GtkWidget **_widget_, double _width_ratio_, double _height_ratio_, gchar *_filename_, ...);
#endif