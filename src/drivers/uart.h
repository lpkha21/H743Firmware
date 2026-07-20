#pragma once

#include <stdint.h>

void uart4Init(uint32_t baudRate);
void uart4WriteByte(uint8_t data);
void uart4WriteString(const char *text);