
#include "Common.h"
#include "Board.h"
#include "Debug.h"

void clocks_start()
{
    NRF_CLOCK->EVENTS_HFCLKSTARTED = 0;
    NRF_CLOCK->TASKS_HFCLKSTART    = 1;
    while (NRF_CLOCK->EVENTS_HFCLKSTARTED == 0);
}

u8 packets[][2] = {
    {0x77, 0x00}, {0xF0, 0x77},
    {0x77, 0x00}, {0xF0, 0x77},
    {0x77, 0x00}, {0xF0, 0x77},
    {0x77, 0x00}, {0xF0, 0x77}
};
u8 packetIndex = 0;

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
        for (u8 i = 2; i < 3; i++) {
            Board::setChannel(i);
            nrf_delay_us(50000);
            Board::keyboard.reset();
            for (u32 j = 0; j < 25000; j++) {
                nrf_delay_us(1000);
                call(Board::keyboard, Board::keyboard.task)();
                if (k++ == 0) {
                    auto p = packets[packetIndex];
                    if (Board::keyboard.sendCode(p[0])) {
                        if (p[1] != 0) Board::keyboard.sendCode(p[1]);
                        packetIndex += 1;
                        if (packetIndex >= asize(packets)) packetIndex = 0;
                    }
                }
            }
        }
    }
}
