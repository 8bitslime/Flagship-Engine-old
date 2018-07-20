#pragma once
#include <stddef.h>
#include <stdint.h>

#ifndef max
#define max(a, b) ((a) < (b) ? (b) : (a))
#endif

#ifndef min
#define min(a, b) ((a) > (b) ? (b) : (a))
#endif

#ifndef false
#define false 0
#endif

#ifndef true
#define true 1
#endif

typedef uint8_t byte_t;
typedef int bool_t;

#define FATAL_ERROR(str, ...) fatal_error(__FILE__ ":\n" str, __VA_ARGS__)
void fatal_error(const char *format, ...);
