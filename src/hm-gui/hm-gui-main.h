#ifndef HM_GUI_MAIN_H
#define HM_GUI_MAIN_H

#include "hm-gui.h"

void on_window_main_signal(GtkWidget *widget, gpointer t_data);
void hm_ui_window_main_initialize();
void hm_ui_window_main_show();
void hm_ui_window_main_hide();
gboolean hm_ui_window_main_is_show();
void hm_ui_window_main_deinitialize();
int8_t hm_ui_window_main_set_text(int idx_t, char text_t, ...);
void hm_ui_window_main_sestive(gboolean flag);
void hm_ui_window_main_start_game();

#endif