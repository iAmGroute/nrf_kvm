
#define nrf_delay_us(us_time) NRFX_DELAY_US(us_time)

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
        nrf_delay_us(1000000);
        Board::Pin_ChannelSwitch.high();
        nrf_delay_us(1000000);
        Board::Pin_ChannelSwitch.low();
    }
}
