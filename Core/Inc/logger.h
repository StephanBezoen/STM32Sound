#ifndef __LOGGER_H__
#define __LOGGER_H__

#include "main.h"

void initLogUart(UART_HandleTypeDef* handle);

void writeLog(char text[]);

#endif // __LOGGER_H__
