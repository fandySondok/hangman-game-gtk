#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include "hm-debug/hm-debug.h"
#include "hm-tools/hm-tools.h"
#include "hm-gui/hm-gui.h"
#include "hm-tools/hm-linked-list.h"
#include "hm-conf/hm-conf.h"

#define MAIN_VERSION "0.01"

static void hm_print_information()
{
  printf("+----------------  Hangman  ----------------+\n"
         "|  Hangman Game Aplication                  |\n"
         "|  COpyright (c) 2022, Fandy Sondok         |\n"
         "|                                           |\n"
         "|  Created by :                             |\n"
         "|        Fandy Sondok :)                    |\n"
         "|  Email  : fandy.sondok@gmail.com          |\n"
         "|  Version: %8s                        |\n"
         "|  Compiled Time :                          |\n"
         "|        %s  %s              |\n"
         "+-------------------------------------------+\n",
         MAIN_VERSION, __DATE__, __TIME__);
}

int main(int argc, char **argv)
{

  hm_print_information();
  hm_tools_set_app_running();

  if (hm_gui_start())
  {
    exit(EXIT_FAILURE);
  }

  while (hm_tools_is_app_ruuning())
  {
    sleep(1);
  }
}
