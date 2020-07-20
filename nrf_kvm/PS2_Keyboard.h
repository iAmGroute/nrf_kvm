#pragma once

#include "PS2.h"

struct PS2_Keyboard : PS2
{
    // A ring buffer that always keeps the last removed element
    struct Buffer
    {
        u8 raw[128];
        u8 i_read  = 0;
        u8 i_write = 0;
        void init()
        {
            i_read  = 0;
            i_write = 0;
        }
        bool isEmpty()
        {
            return i_read == i_write;
        }
        bool push(u8 d)
        {
            u8 i_next = (i_write + 1) % 128;
            if (i_next == i_write) return false;
            raw[i_write] = d;
            i_write = i_next;
            return true;
        }
        u8 current()
        {
            return raw[i_read];
        }
        u8 previous()
        {
            return raw[(i_read - 1) % 128];
        }
        u8 pop()
        {
            u8 d   = raw[i_read];
            i_read = (i_read + 1) % 128;
            return d;
        }
    };

    bool   streaming = false;
    u8     typematic = 0;
    u8     codeset   = 0;
    u8     leds      = 0;
    u8     cmd       = 0;
    Buffer buffer;

    PS2_Keyboard(System::GPIO::Pin clock_pin, System::GPIO::Pin data_pin)
        : PS2(clock_pin, data_pin)
    { }

    void defaults()
    {
        typematic = 0;
        codeset   = 0;
        leds      = 0;
    }

    async reset(Async2 *pt2)
    {
        async_begin(pt2);
        p('r'); p('\n');
        defaults();
        streaming = true;
        buffer.init();
        clock_pin.high();
        data_pin.high();
        nrf_delay_us(1000);
        await_call(insistWrite, &pt2->pt, 0xAA);
        p('R'); p('\n');
        async_end;
    }

    async _respond(Async3 *pt3)
    {
        async_begin(pt3);
        p('_'); p('\n');
        await_call(readByte,        &pt3->pt2.pt, cmd);
        if (cmd == 0) async_exit;
        buffer.init(); // do we have to ? :(
        await_call(ack,             &pt3->pt2.pt);
        if (cmd == 0xFF) {
            // just reset
            await_call(reset,       &pt3->pt2);
        }
        if (cmd == 0xFE) {
            // resend last byte
            await_call(insistWrite, &pt3->pt2.pt, buffer.previous());
        }
        else if (cmd == 0xF6) {
            // set default
            defaults();
        }
        else if (cmd == 0xF5) {
            // disable & set default
            streaming = false;
            defaults();
        }
        else if (cmd == 0xF4) {
            // enable
            streaming = true;
        }
        else if (cmd == 0xF3) {
            // set typematic
            await_call(readByte,    &pt3->pt2.pt, typematic);
            await_call(ack,         &pt3->pt2.pt);
        }
        else if (cmd == 0xF2) {
            // get device ID
            await_call(insistWrite, &pt3->pt2.pt, 0xAB);
            await_call(insistWrite, &pt3->pt2.pt, 0x83); // keyboard
        }
        else if (cmd == 0xF0) {
            // set scan code set
            await_call(readByte,    &pt3->pt2.pt, codeset);
            await_call(ack,         &pt3->pt2.pt);
            if (codeset == 0) await_call(insistWrite, &pt3->pt2.pt, 2);
        }
        else if (cmd == 0xEE) {
            // echo
            await_call(insistWrite, &pt3->pt2.pt, 0xEE);
        }
        else if (cmd == 0xED) {
            // set LEDs
            await_call(readByte,    &pt3->pt2.pt, leds);
            await_call(ack,         &pt3->pt2.pt);
        }
        else if (cmd != 0) {
            // unknown command
            await_call(reset,       &pt3->pt2);
        }
        async_end;
    }
    async task(Async3 *pt3)
    {
        if (!async_done(pt3)) return _respond(pt3);
        if (data_pin.isLow()) {
            async_init(pt3);
            return _respond(pt3);
        }
        else if (clock_pin.isHigh()) {
            if (streaming and !buffer.isEmpty()) {
                p('p');
                bool ok = writeByte(buffer.current());
                p('\n');
                if (ok) buffer.pop();
                else {
                    async_init(pt3);
                    return _respond(pt3);
                }
            }
        }
        return ASYNC_DONE;
    }

    bool sendCode(u8 code)
    {
        return buffer.push(code);
    }
};
