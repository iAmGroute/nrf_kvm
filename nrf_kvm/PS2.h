#pragma once

#include "Common.h"
#include <lib/System/GPIO.h>
#include "Debug.h"

struct Interface_PS2
{
    System::GPIO::Pin clock_pin;
    System::GPIO::Pin data_pin;
    bool streaming = false;
    u8 packetData[8];
    u8 packetSize = 0;
    u8 sendIndex  = 0;
    u8 sampleRate = 0;
    u8 resolution = 0;
    void (Interface_PS2::*task)() = &Interface_PS2::_task;

    static void configPin(System::GPIO::Pin &pin)
    {
        pin.setStrength(System::GPIO::Strength::Standard_Low_Open_High);
        //pin.setPull(System::GPIO::Pull::Up);
        pin.high();
        pin.setOutput();
        pin.connectInputBuffer();
    }

    Interface_PS2(System::GPIO::Pin clock_pin, System::GPIO::Pin data_pin)
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
        if (!writeBit(0))      return false;
        u8 parity = 1;
        for (u8 i = 0; i < 8; i++) {
            u8 b     = data & 0x01;
            data   >>= 1;
            parity  ^= b;
            if (!writeBit(b)) return false;
        }
        if (!writeBit(parity)) return false;
        writeBit(1);
        return true;
    }
    bool writeByte(u8 data) {
        p('w'); p8(data);
        bool ok = _writeByte(data);
        if (!ok) data_pin.high();
        p8(ok);
        return ok;
    }
    void insistWrite(u8 data) {
        p('i'); p8(data);
        while (!_writeByte(data)) data_pin.high();
        p('-'); p('\n');
    }

    u8 readBit()
    {
        pulse();
        return data_pin.isHigh() ? 1 : 0;
    }

    u8 readByte()
    {
        p('r');
        while ( data_pin.isHigh()) if (clock_pin.isHigh()) return 0;
        p('+');
        while (clock_pin.isLow());
        p('+');
        u8 data   = 0;
        u8 parity = 1;
        for (u8 i = 0; i < 8; i++) {
            data >>= 1;
            if (readBit()) {
                data   |= 0x80;
                parity ^= 1;
            }
        }
        u8 temp = readBit();
        p8(data);
        if (temp != parity)         p('p');
        while (readBit() != 0x01) { p('s'); nrf_delay_us(100); }
        writeBit(0);
        data_pin.high();
        p('\n');
        nrf_delay_us(40);
        return data;
    }

    void ack() { insistWrite(0xFA); }

    void reset()
    {
        p('r'); p('\n');
        streaming  = false;
        packetSize = 0;
        sendIndex  = 0;
        sampleRate = 0;
        resolution = 0;
        clock_pin.high();
        data_pin.high();
        nrf_delay_us(1000);
        insistWrite(0xAA);
        insistWrite(0x00);
        p('R'); p('\n');
    }

    void _respond_sampleRate_1()
    {
        if (clock_pin.isLow()) {
            sampleRate = readByte();
            ack();
            task = &Interface_PS2::_task;
        }
    }
    void _respond_resolution_1()
    {
        if (clock_pin.isLow()) {
            resolution = readByte();
            ack();
            task = &Interface_PS2::_task;
        }
    }
    void _respond()
    {
        p('_'); p('\n');
        u8 cmd = readByte();
        if (cmd == 0) return;
        ack();
        if (cmd == 0xFF) {
            reset();
        }
        else if (cmd == 0xF2) {
            // get device ID
            insistWrite(0x00); // (simple mouse)
        }
        else if (cmd == 0xF3) {
            // set sample rate
            task = &Interface_PS2::_respond_sampleRate_1;
        }
        else if (cmd == 0xE8) {
            // set resolution
            task = &Interface_PS2::_respond_resolution_1;
        }
        else if (cmd == 0xE6) {
            // set scaling 1:1
        }
        else if (cmd == 0xF4) {
            // enable data reporting
            sendIndex = 0;
            streaming = true;
        }
        else if (cmd != 0) {
            // unknown command
            reset();
        }
    }
    void _task()
    {
        if (clock_pin.isLow()) _respond();
        else if (packetSize > 0) {
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
                _respond();
            }
        }
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
