
#include "Board.h"

#define nrf_delay_us(us_time) nrfx_coredep_delay_us(us_time)

void clocks_start()
{
    NRF_CLOCK->EVENTS_HFCLKSTARTED = 0;
    NRF_CLOCK->TASKS_HFCLKSTART    = 1;
    while (NRF_CLOCK->EVENTS_HFCLKSTARTED == 0);
}

int main()
{
    clocks_start();
    Board::configPins();

    while (1) {
        for (u8 i = 0; i < 4; i++) {
            Board::setChannel(i);
            nrf_delay_us(500000);
        }
    }
}
