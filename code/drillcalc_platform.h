#ifndef DRILL_CALC_H

typedef int32_t int32;
typedef int16_t int16;
typedef int8_t int8;
typedef uint32_t uint32;
typedef uint16_t uint16;
typedef uint8_t uint8;
typedef float real32;
typedef double real64;
typedef int32 bool32;

#define ArrayCount(Array) (sizeof((Array)) / sizeof((Array)[0]))

#define global_variable static
#define local_persist static
#define internal static

#define DRILL_CALC_H
#endif
