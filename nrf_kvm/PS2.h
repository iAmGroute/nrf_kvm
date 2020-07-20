#pragma once

#include "Common.h"
#include <lib/System/GPIO.h>
#include "Debug.h"

struct PS2
{
    System::GPIO::Pin clock_pin;
    System::GPIO::Pin data_pin;

    static void configPin(System::GPIO::Pin &pin)
    {
        pin.setStrength(System::GPIO::Strength::Standard_Low_Open_High);
        pin.setPull(System::GPIO::Pull::Up);
        pin.high();
        pin.setOutput();
        pin.connectInputBuffer();
    }

    PS2(System::GPIO::Pin clock_pin, System::GPIO::Pin data_pin)
        : clock_pin(clock_pin), data_pin(data_pin)
    {
        configPin(clock_pin);
        configPin(data_pin);
    }

    bool pulse() {
        nrf_delay_us(20);
        clock_pin.low();
        nrf_delay_us(40);
        clock_pin.high();
        nrf_delay_us(20);
        return clock_pin.isHigh();
    }

    bool writeBit(u8 b) {
        if (b == 0) data_pin.low();
        else        data_pin.high();
        return pulse();
    }
    bool _writeByte(u8 data)
    {
        if (clock_pin.isLow()) return false;
        if ( data_pin.isLow()) return false;
        bool ok = writeBit(0);
        if (!ok) {
            data_pin.high();
            return false;
        }
        u8 parity = 1;
        for (u8 i = 0; i < 8; i++) {
            u8 b     = data & 0x01;
            data   >>= 1;
            parity  ^= b;
            // if (!writeBit(b)) return false;
            writeBit(b);
        }
        // if (!writeBit(parity)) return false;
        writeBit(parity);
        writeBit(1);
        return true;
    }
    bool writeByte(u8 data) {
        p('w'); p8(data);
        bool ok = _writeByte(data);
        p8(ok);
        return ok;
    }
    async insistWrite(Async *pt, u8 data) {
        async_begin(pt);
        p('i'); p8(data);
        await(_writeByte(data));
        p('-'); p('\n');
        async_end;
    }

    u8 readBit()
    {
        pulse();
        return data_pin.isHigh() ? 1 : 0;
    }
    async readByte(Async *pt, u8 &res)
    {
        async_begin(pt);
        p('r');
        await(data_pin.isLow() or clock_pin.isHigh());
        if (clock_pin.isHigh()) {
            res = 0;
            async_exit;
        }
        p('+');
        await(clock_pin.isHigh());
        p('+');
        {
            u8 data   = 0;
            u8 parity = 1;
            for (u8 i = 0; i < 8; i++) {
                data >>= 1;
                if (readBit()) {
                    data   |= 0x80;
                    parity ^= 1;
                }
            }
            if (readBit() != parity) p('p');
            if (readBit() != 0x01)   p('s');
            writeBit(0);
            data_pin.high();
            p8(data);
            nrf_delay_us(40);
            p('\n');
            res = data;
        }
        async_end;
    }

    async ack(Async *pt)
    {
        return insistWrite(pt, 0xFA);
    }
};
