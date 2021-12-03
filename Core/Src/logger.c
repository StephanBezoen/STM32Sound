#include "logger.h"
#include <stdio.h>
#include <string.h>

#define PENDING_MESSAGES_BUFFER_SIZE 8192

char messageBuffer[PENDING_MESSAGES_BUFFER_SIZE];
uint readPointer = 0;
uint writePointer = 0;
UART_HandleTypeDef *uartHandle;

void initLogUart(UART_HandleTypeDef *handle) { uartHandle = handle; }

void checkSendNextMessage() {
    if (uartHandle->gState != HAL_UART_STATE_READY) return; // Transmission in progress

    if (readPointer == writePointer) return; // All messages sent

    int messageStart = (uint)messageBuffer + readPointer;

    int messageLength = 0;
    if (readPointer > writePointer) { // Read pointer is beyond write pointer, we'll do this in 2 batches
        messageLength = PENDING_MESSAGES_BUFFER_SIZE - readPointer;
        readPointer = 0;
    } else { // Read pointer is before write pointer, send all bytes in between
        messageLength = writePointer - readPointer;
        readPointer = writePointer;
    }

    HAL_UART_Transmit_IT(uartHandle, (uint8_t *)messageStart, messageLength);
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) { checkSendNextMessage(); }

void appendMessageText(char *message) {
    int messageLength = strlen(message);
    
    int bufferSizeLeft = (PENDING_MESSAGES_BUFFER_SIZE - writePointer);
    if (messageLength > bufferSizeLeft) {
        int len1 = bufferSizeLeft;
        int len2 = messageLength - bufferSizeLeft;
        memcpy(messageBuffer + writePointer, message, len1);
        memcpy(messageBuffer, message + len1, len2);
        writePointer = len2;
    } else {
        memcpy(messageBuffer + writePointer, message, messageLength);
        writePointer += messageLength;
    }
}

void writeLog(char text[]) {
    // message
    appendMessageText(text);

    // timestamp
    char buffer[32];
    snprintf(buffer, sizeof(buffer), " - %8ld", HAL_GetTick());
    appendMessageText(buffer);

    // newline
    appendMessageText("\r\n");

    // Send if possible
    checkSendNextMessage();
}
