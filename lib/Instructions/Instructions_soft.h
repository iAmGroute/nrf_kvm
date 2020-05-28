//! To be included only by 'Instructions.h'

#pragma once

void NOP() {}

// Get bit VALue
#define VAL(word, position)            ((word) &  (1UL << (position)))
// Set Bit In
#define SBI(word, position)            ((word) |  (1UL << (position)))
// Clear Bit In
#define CBI(word, position)            ((word) & ~(1UL << (position)))
// Bit Field Extract
#define BFE(word, offset, mask)        (((word) &  (mask)) >> (offset))
// Bit Field Insert
#define BFI(word, offset, mask, field) (((word) & ~(mask)) | (((field) << (offset)) & (mask)))

// ROtate Right
u32 ROR(u32 number, u32 times)
{
    times %= 32;
    return (number >> times) | (number << (32 - times));
}

u32 CTZ(u32 number)
{
    u32 i = 0;
    while (number != 0) {
        if (number & 0x1) return i;
        number >>= 1;
        i += 1;
    }
    return 32;
}

u32 CLZ(u32 number)
{
    u32 i = 0;
    while (number != 0) {
        if (number & 0x80000000) return i;
        number <<= 1;
        i += 1;
    }
    return 32;
}
