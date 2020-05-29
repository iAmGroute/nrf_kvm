
#include "Common.h"
#include "Board.h"

void clocks_start()
{
    NRF_CLOCK->EVENTS_HFCLKSTARTED = 0;
    NRF_CLOCK->TASKS_HFCLKSTART    = 1;
    while (NRF_CLOCK->EVENTS_HFCLKSTARTED == 0);
}

s8 packets[][3] = {
    {0x00,  32,   0},
    {0x00,   0,  32},
    {0x00, -32,   0},
    {0x00,   0, -32}
};
u8 packetIndex = 0;

int main()
{
    clocks_start();
    Board::configPins();

    while (1) {
        u8 k = 0;
        for (u8 i = 2; i < 3; i++) {
            Board::setChannel(i);
            nrf_delay_us(500000);
            Board::mouse.reset();
            for (u32 j = 0; j < 25000; j++) {
                nrf_delay_us(1000);
                call(Board::mouse, Board::mouse.task)();
                if (k++ == 0) {
                    auto p = packets[packetIndex];
                    if (Board::mouse.sendPacket((u8)p[0], p[1], p[2])) {
                        packetIndex += 1;
                        if (packetIndex >= asize(packets)) packetIndex = 0;
                    }
                }
            }
        }
    }
}
