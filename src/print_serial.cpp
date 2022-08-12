#include "print_serial.h"
#include <stdio.h>


void _p_default_print_handler(const char *){
        return;
}

void (*p_print)(const char *) = _p_default_print_handler;

void p(const char *fmt, ... ){
        char buff[SERIAL_PRINT_MAX_BUFF_SIZE]; // resulting string limited to 128 chars
        va_list args;
        va_start (args, fmt );
        vsnprintf(buff, sizeof(buff), fmt, args);
        va_end (args);
        (*p_print)(buff);
}

void p_registerPrint( void (*print)(const char *) ){
        p_print = print;
}