#pragma once

namespace System {

    enum struct PinNumber : u32 {
        None  = (u32)~0U,
        P0_00 =  0,
        P0_01 =  1,
        P0_02 =  2,
        P0_03 =  3,
        P0_04 =  4,
        P0_05 =  5,
        P0_06 =  6,
        P0_07 =  7,
        P0_08 =  8,
        P0_09 =  9,
        P0_10 = 10,
        P0_11 = 11,
        P0_12 = 12,
        P0_13 = 13,
        P0_14 = 14,
        P0_15 = 15,
        P0_16 = 16,
        P0_17 = 17,
        P0_18 = 18,
        P0_19 = 19,
        P0_20 = 20,
        P0_21 = 21,
        P0_22 = 22,
        P0_23 = 23,
        P0_24 = 24,
        P0_25 = 25,
        P0_26 = 26,
        P0_27 = 27,
        P0_28 = 28,
        P0_29 = 29,
        P0_30 = 30,
        P0_31 = 31
    };

    // The following doesn't work :(
    // so that's why PinNumber is u32.
    // operator u32(PinNumber p)
    // {
    //     return p == PinNumber::None ? (u32)(~0UL) : (u32)p;
    // }

    constexpr u8 PinCount = 32;
}
