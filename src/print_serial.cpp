#include "print_serial.h"
#include <stdio.h>

void (*p_print)(char *);

void p(char *fmt, ... ){
        char buff[SERIAL_PRINT_MAX_BUFF_SIZE]; // resulting string limited to 128 chars
        va_list args;
        va_start (args, fmt );
        vsnprintf(buff, sizeof(buff), fmt, args);
        va_end (args);
        (*p_print)(buff);
}

void p_registerPrint( void (*print)(char *) ){
        p_print = print;
}