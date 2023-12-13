#ifndef HM_TOOLS_H
#define HM_TOOLS_H

typedef enum
{
  APP_STOP = 0,
  APP_CATCH_STOP,
  APP_RUN
} hm_tools_app_state_t;

hm_tools_app_state_t hm_tools_is_app_ruuning();
void hm_tools_set_app_running();
void hm_tools_set_app_stop();
void enco_tools_signal_interrupt(int sig);
int hm_tools_centering_word(char *source_t, char *dest_t, int flag_ubah);
void hm_tools_change_word(char *source_t, char *dest_t);
int8_t hm_tools_guessing_word(char *show_word_t, char *guess_word_t, char guess_char);
int8_t hm_tools_check_won_game(char *show_word_t);

#endif