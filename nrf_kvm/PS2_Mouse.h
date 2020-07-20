#pragma once

#include "PS2.h"

struct PS2_Mouse : PS2
{
    bool streaming = false;
    u8 packetData[8];
    u8 packetSize = 0;
    u8 sendIndex  = 0;
    u8 sampleRate = 0;
    u8 resolution = 0;
    u8 cmd        = 0;
    async (PS2_Mouse::*task)(Async3 *pt3) = &PS2_Mouse::_task;

    PS2_Mouse(System::GPIO::Pin clock_pin, System::GPIO::Pin data_pin)
        : PS2(clock_pin, data_pin)
    { }

    async reset(Async2 *pt2)
    {
        async_begin(pt2);
        p('#'); p('\n');
        streaming  = false;
        packetSize = 0;
        sendIndex  = 0;
        sampleRate = 0;
        resolution = 0;
        task       = &PS2_Mouse::_task;
        clock_pin.high();
        data_pin.high();
        nrf_delay_us(1000);
        await_call(insistWrite, &pt2->pt, 0xAA);
        await_call(insistWrite, &pt2->pt, 0x00);
        p('$'); p('\n');
        async_end;
    }

    async _respond_sampleRate_1(Async3 *pt3)
    {
        async_begin(pt3);
        if (data_pin.isLow()) {
            await_call(readByte, &pt3->pt2.pt, sampleRate);
            await_call(ack,      &pt3->pt2.pt);
            task = &PS2_Mouse::_task;
        }
        async_end;
    }
    async _respond_resolution_1(Async3 *pt3)
    {
        async_begin(pt3);
        if (data_pin.isLow()) {
            await_call(readByte, &pt3->pt2.pt, resolution);
            await_call(ack,      &pt3->pt2.pt);
            task = &PS2_Mouse::_task;
        }
        async_end;
    }
    async _respond(Async3 *pt3)
    {
        async_begin(pt3);
        p('_'); p('\n');
        await_call(readByte,        &pt3->pt2.pt, cmd);
        p8(cmd);
        if (cmd == 0) async_exit;
        await_call(ack,             &pt3->pt2.pt);
        if (cmd == 0xFF) {
            // just reset
            await_call(reset,       &pt3->pt2);
        }
        else if (cmd == 0xF4) {
            // enable data reporting
            sendIndex = 0;
            streaming = true;
        }
        else if (cmd == 0xF3) {
            // set sample rate
            task = &PS2_Mouse::_respond_sampleRate_1;
        }
        else if (cmd == 0xF2) {
            // get device ID
            await_call(insistWrite, &pt3->pt2.pt, 0x00); // (simple mouse)
        }
        else if (cmd == 0xE8) {
            // set resolution
            task = &PS2_Mouse::_respond_resolution_1;
        }
        else if (cmd == 0xE6) {
            // set scaling 1:1
        }
        else if (cmd != 0) {
            // unknown command
            await_call(reset,       &pt3->pt2);
        }
        async_end;
    }
    async _task(Async3 *pt3)
    {
        if (!async_done(pt3)) return _respond(pt3);
        if (data_pin.isLow()) {
            async_init(pt3);
            return _respond(pt3);
        }
        else if (clock_pin.isHigh()) {
            if (streaming && packetSize > 0) {
                p('p');
                bool ok = writeByte(packetData[sendIndex]);
                p('\n');
                if (ok) {
                    sendIndex += 1;
                    if (sendIndex >= packetSize) {
                        sendIndex  = 0;
                        packetSize = 0;
                    }
                }
                else {
                    sendIndex = 0;
                    async_init(pt3);
                    return _respond(pt3);
                }
            }
        }
        return ASYNC_DONE;
    }

    bool sendPacket(u8 control, s8 dx, s8 dy)
    {
        if (streaming and packetSize == 0) {
            packetData[0] = control | 0x08
                          | (dx < 0 ? 0x10 : 0)
                          | (dy < 0 ? 0x20 : 0);
            packetData[1] = (u8)dx;
            packetData[2] = (u8)dy;
            packetSize    = 3;
            return true;
        }
        return false;
    }
};
