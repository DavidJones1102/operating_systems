#define BUFF_SIZE 1024
#define TOK_DELIM " \t\r\n\a"
#ifdef USE_DLL
    #include "wclib_so.h"
#else
    #include "wclib.h"
#endif
#include <ctype.h>
#include <time.h>
#include <sys/times.h>
#include <dlfcn.h>

enum command{INVALID, INIT, COUNT, SHOW, DELETE, DESTROY};
enum command convert(char* str);
char* read_line();
char** split_line(char* line);
void execute(enum command to_exec, char* parameters);
void check_and_run(char** words);
int is_number(char* str);