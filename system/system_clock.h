#pragma once

#include <stdint.h>

#define SYSTEM_CPU_CLOCK_HZ  200000000UL
#define SYSTEM_HCLK_HZ       100000000UL
#define SYSTEM_PCLK1_HZ       50000000UL
#define SYSTEM_PCLK2_HZ       50000000UL
#define SYSTEM_PCLK3_HZ       50000000UL
#define SYSTEM_PCLK4_HZ       50000000UL

extern uint32_t SystemCoreClock;