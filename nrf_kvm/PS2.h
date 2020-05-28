
#include "Board.h"

struct Interface_PS2
{
    const Pin clock_pin;
    const Pin data_pin;

    Interface_PS2(Pin clock_pin, Pin data_pin)
        : clock_pin(clock_pin), data_pin(data_pin)
    {
        clock_pin.high();
        data_pin.high();
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
        bool ok = _writeByte(data);
        if (!ok) data_pin.high();
        return ok;
    }
    void insistWrite(u8 data) { while (!_writeByte(data)) data_pin.high(); }

    u8 readBit()
    {
        pulse();
        return data_pin.isHigh() ? 1 : 0;
    }

    u8 readByte()
    {
        while ( data_pin.isHigh()) if (clock_pin.isHigh()) return 0;
        while (clock_pin.isLow());
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
        // if (temp != parity) { _print("! Parity mismatch, got: "); _println(temp); }
        while (readBit() != 0x01); // _println("! Stop bit mismatch");
        writeBit(0);
        data_pin.high();
        nrf_delay_us(40);
        return data;
    }

    void ack() { insistWrite(0xFA); }

    void reset()
    {
        clock_pin.high();
        data_pin.high();
        nrf_delay_us(1000);
        insistWrite(0xAA);
        insistWrite(0x00);
    }
}

    if (clock_pin.isLow()) {
        Serial.write('.');
        // _print("Arb: clk="); _print(r(clk)); _print(", dat="); _print(r(dat)); _println(".");
        u8 cmd = readByte();
        if (cmd == 0xFF) {
            ack();
            // _println("! RESETING\n");
            reset();
            return;
        }
        else if (cmd == 0xF2) {
            ack();
            insistWrite(0x00); // device ID (simple mouse)
        }
        else if (cmd == 0xF3) {
            ack();
            while (clock_pin.isHigh());
            readByte(); // sample rate
            ack();
        }
        else if (cmd == 0xE8) {
            ack();
            while (clock_pin.isHigh());
            readByte(); // resolution
            ack();
        }
        else if (cmd == 0xE6) {
            // set scaling 1:1
            ack();
        }
        else if (cmd == 0xF4) {
            // enable data reporting
            ack();
            tosend[2] = 0x00;
            tosend[1] = delta;
            tosend[0] = delta;
            tosendCount = 3;
            sendIndex = 0;
        }
        else if (cmd == 0) return;
        else {
            // _print("\n\n UNKNOWN COMMAND: ");
            // _printx(cmd);
            // _println(" ~~~~~~~~~~~~\n\n");
            // digitalWrite(13, HIGH);
        }
    }
    if (tosendCount > 0) {
        if (sendIndex >= tosendCount) sendIndex = 0;
        if (writeByte(tosend[sendIndex])) sendIndex += 1;
        else sendIndex = 0;
    }
