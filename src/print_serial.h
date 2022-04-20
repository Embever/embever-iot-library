#ifndef INC_PRINT_SERIAL_H
#define INC_PRINT_SERIAL_H

#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>

#define SERIAL_PRINT_MAX_BUFF_SIZE 150

void p(const char *fmt, ... );
void p_registerPrint( void (*print)(const char *) );

#define LOG_SETUP_ARDUINO       void log_print(const char *str){ Serial.print(str); }
#define LOG_REGISTER_ARDUINO    p_registerPrint(log_print);

#endif