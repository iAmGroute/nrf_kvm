//! To be included only by 'Instructions.h'

#pragma once

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
    // Since Rs is not immediate, Rd must be the same as Rm
    asm ("RORS %[Rd], %[Rs]" : [Rd] "=r" (number) : [Rs] "r" (times));
    return number;
}
/*
u32 ROR_Imm(u32 number, u32 times)
{
    // Since Is is immediate, <times> must be compile-time constant
    u32 result;
    asm ("RORS %[Rd], %[Rm], %[Is]" : [Rd] "=r" (result) : [Rm] "r" (number), [Is] "i" (times));
    return number;
}
*/

u32 CTZ(u32 number)
{
    u32 i = 0;
    while (number) {
        if (number & 0x1) return i;
        number >>= 1;
        i += 1;
    }
    return 32;
}

u32 CLZ(u32 number)
{
    u32 i = 0;
    while (number) {
        if (number & 0x80000000) return i;
        number <<= 1;
        i += 1;
    }
    return 32;
}
