
bool keepChannel = false;

#include "Common.h"
#include "Board.h"
#include "Debug.h"

void clocks_start()
{
    NRF_CLOCK->EVENTS_HFCLKSTARTED = 0;
    NRF_CLOCK->TASKS_HFCLKSTART    = 1;
    while (NRF_CLOCK->EVENTS_HFCLKSTARTED == 0);
}

u8 kb_packets[][2] = {
    {0x77, 0x00}, {0xF0, 0x77},
    {0x77, 0x00}, {0xF0, 0x77},
    {0x77, 0x00}, {0xF0, 0x77},
    {0x77, 0x00}, {0xF0, 0x77}
};
s8 ms_packets[][3] = {
    {0x00,  32,   0},
    {0x00,   0,  32},
    {0x00, -32,   0},
    {0x00,   0, -32}
};
u8 kb_packetIndex = 0;
u8 ms_packetIndex = 0;

Async3 pt3_kb;
Async3 pt3_ms;

int main()
{
    clocks_start();
    Board::init();

    p('\n');
    p('\n');
    p('\n');
    p('\n');
    p('\n');
    p('\n');
    p('\n');

    while (1) {
        u8 k = 0;
        for (u8 i = 0; i < 4; i++) {
            Board::setChannel(i);
            nrf_delay_us(50000);
            // Board::keyboard.reset();
            // Board::mouse.reset();
            keepChannel = true;
            while (true) {
                if (Board::Pin_Switch.isLow()) {
                    nrf_delay_us(250000);
                    while (Board::Pin_Switch.isLow());
                    p('s');
                    break;
                }
                nrf_delay_us(500);
                while (1) {
                    call(Board::keyboard, Board::keyboard.task)(&pt3_kb);
                    call(Board::mouse,    Board::mouse.task   )(&pt3_ms);
                }
                if (k++ == 0 and false) {
                    {
                        auto p = kb_packets[kb_packetIndex];
                        if (Board::keyboard.sendCode(p[0])) {
                            if (p[1] != 0) Board::keyboard.sendCode(p[1]);
                            kb_packetIndex += 1;
                            if (kb_packetIndex >= asize(kb_packets)) kb_packetIndex = 0;
                        }
                    }
                    {
                        auto p = ms_packets[ms_packetIndex];
                        if (Board::mouse.sendPacket((u8)p[0], p[1], p[2])) {
                            ms_packetIndex += 1;
                            if (ms_packetIndex >= asize(ms_packets)) ms_packetIndex = 0;
                        }
                    }
                }
            }
        }
    }
}
