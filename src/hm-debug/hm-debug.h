#ifndef HM_DEBUG_H
#define HM_DEBUG_H

// https://stackoverflow.com/a/1961222
#define RESET "\033[0m"
#define BLACK "\033[30m"              /* Black */
#define RED "\033[31m"                /* Red */
#define GREEN "\033[32m"              /* Green */
#define YELLOW "\033[33m"             /* Yellow */
#define BLUE "\033[34m"               /* Blue */
#define MAGENTA "\033[35m"            /* Magenta */
#define CYAN "\033[36m"               /* Cyan */
#define WHITE "\033[37m"              /* White */
#define BOLDBLACK "\033[1m\033[30m"   /* Bold Black */
#define BOLDRED "\033[1m\033[31m"     /* Bold Red */
#define BOLDGREEN "\033[1m\033[32m"   /* Bold Green */
#define BOLDYELLOW "\033[1m\033[33m"  /* Bold Yellow */
#define BOLDBLUE "\033[1m\033[34m"    /* Bold Blue */
#define BOLDMAGENTA "\033[1m\033[35m" /* Bold Magenta */
#define BOLDCYAN "\033[1m\033[36m"    /* Bold Cyan */
#define BOLDWHITE "\033[1m\033[37m"   /* Bold White */

#define KNRM "\x1B[0m"
#define KRED "\x1B[31m"
#define KGRN "\x1B[32m"
#define KYEL "\x1B[33m"
#define KBLU "\x1B[34m"
#define KMAG "\x1B[35m"
#define KCYN "\x1B[36m"
#define KWHT "\x1B[37m"

#define BLINK "\x1b[5m"
#define UNDERSCORE "\x1b[4m"

#define BGblack "\x1b[40m"
#define BGred "\x1b[41m"
#define BGgreen "\x1b[42m"
#define BGyellow "\x1b[43m"
#define BGblue "\x1b[44m"
#define BGmagenta "\x1b[45m"
#define BGcyan "\x1b[46m"
#define BGwhite "\x1b[47m"

int debug(const char *function_name, char *debug_type, char *debug_msg, ...);

#endif