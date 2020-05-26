#pragma once

#ifdef __ARM_ARCH_6M__
    void HardFault_Handler();
    #ifdef DEBUG
        #define assert(condition) { if (!(condition)) HardFault_Handler(); }
    #else
        #define assert(condition)
    #endif
#else
    #define assert(condition)
#endif

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t  s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef u8 byte;

#define K (1000UL)
#define M (1000ULL * Ki)
#define G (1000ULL * Mi)
#define T (1000ULL * Gi)

#define Ki (1024UL)
#define Mi (1024ULL * Ki)
#define Gi (1024ULL * Mi)
#define Ti (1024ULL * Gi)
