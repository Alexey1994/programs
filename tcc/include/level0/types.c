#ifndef TYPES_INCLUDED
#define TYPES_INCLUDED


typedef unsigned char          Byte;
typedef unsigned char          Bit8;
typedef unsigned short int     Bit16;
typedef unsigned long int      Bit32;
typedef unsigned long long int Bit64;

typedef unsigned int           Number;
typedef unsigned char          Number8;
typedef unsigned short int     Number16;
typedef unsigned long int      Number32;

typedef int                    Integer_Number;
typedef char                   Integer_Number8;
typedef short int              Integer_Number16;
typedef long int               Integer_Number32;

typedef float                  Rational_Number;
typedef float                  Rational_Number32;
typedef double                 Rational_Number64;

typedef Number                 Boolean;

typedef struct
{
	Number32 value_low;
	Number32 value_high;
}
Number64;


#define stdcall      __attribute__((__stdcall__))
#define cdecl        __attribute__((__cdecl__))
#define import       __attribute__((dllimport)) __attribute__((__stdcall__))
#define import_cdecl __attribute__((dllimport)) __attribute__((__cdecl__))
#define export       __attribute__((dllexport)) __attribute__((__stdcall__))
#define export_cdecl __attribute__((dllexport)) __attribute__((__cdecl__))


#pragma pack(push, 1) //no align for structs


#include <log.c>


#endif//TYPES_INCLUDED