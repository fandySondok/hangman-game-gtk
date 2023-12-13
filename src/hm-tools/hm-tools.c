#include <stdlib.h>
#include <sys/signal.h>

#include "hm-tools/hm-tools.h"
#include "hm-debug/hm-debug.h"
#include "hm-gui/hm-gui.h"

hm_tools_app_state_t MAIN_APP_STATE = APP_RUN;

hm_tools_app_state_t hm_tools_is_app_ruuning()
{
  return MAIN_APP_STATE;
}

void hm_tools_set_app_running()
{
  MAIN_APP_STATE = APP_RUN;
}

void hm_tools_set_app_stop()
{
  MAIN_APP_STATE = APP_STOP;
}

void enco_tools_signal_interrupt(int sig)
{
  switch (sig)
  {
  case SIGINT:
    printf("Request termination : Ctrl+C.\n");
  case SIGTERM:
    printf("Request termination from user.\n");
    // ui_exit();
    MAIN_APP_STATE = APP_STOP;
    break;
  case SIGABRT:
    debug(__func__, "ERROR:", "abortion detected!");
  case SIGSEGV:
    debug(__func__, "ERROR:", "segmentation fault detected!");
    // enco_tools_set_app_stop();
    sleep(1);
    exit(EXIT_FAILURE);
    break;
  default:
    printf("Catch signal unknown.\n");
    break;
  }
}

int hm_tools_centering_word(char *source_t, char *dest_t, int flag_ubah)
{
  int source_len_t = strlen(source_t);
  if (source_len_t > 20)
    return -1;
  char tmp_word[20];
  memset(tmp_word, ' ', sizeof(tmp_word));
  memcpy(tmp_word + (10 - (source_len_t / 2)), source_t, source_len_t);
  if (flag_ubah)
  {
    for (int i = 0; i < 20; i++)
    {
      if (tmp_word[i] != ' ')
        tmp_word[i] = '_';
    }
  }
  memset(tmp_word + 20, 0x00, 1);
  strcpy(dest_t, tmp_word);
  return 0;
}

void hm_tools_change_word(char *source_t, char *dest_t)
{
  for (int i = 0; i < 20; i++)
  {
    if (source_t[i] != ' ')
      source_t[i] = '.';
  }
  strcpy(dest_t, source_t);
}

int8_t hm_tools_guessing_word(char *show_word_t, char *guess_word_t, char guess_char)
{
  int8_t flag_right_answer = 0;

  int gw_len = strlen(guess_word_t);
  int start_show = 0;
  char temporary_show_word[20];
  memset(temporary_show_word, 0x00, sizeof(temporary_show_word));
  strcpy(temporary_show_word, show_word_t);

  while (strlen(show_word_t) > start_show)
  {
    if (show_word_t[start_show] != ' ')
      break;
    start_show++;
  }

  for (int i = 0; i < gw_len; i++)
  {
    if (i == 0)
      guess_char -= 32;
    else if (i == 1)
      guess_char += 32;

    if (guess_char == guess_word_t[i])
    {
      temporary_show_word[i + start_show] = guess_char;
      flag_right_answer = 1;
    }
  }
  strcpy(show_word_t, temporary_show_word);

  if (flag_right_answer)
    return 0;
  return -1;
}

int8_t hm_tools_check_won_game(char *show_word_t)
{
  for (int i = 0; i < 20; i++)
  {
    if (show_word_t[i] == '_')
    {
      return 1;
    }
  }
  return 0;
}