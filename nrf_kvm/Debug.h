#pragma once

#include <lib/System/Peripherals/UART.h>

using namespace System::Peripherals;

void p(char c) {
    UART0.write(c);                   while (!UART0.checkAndClearEvent(UART::Event::TX_Ready));
    UART0.write(' ');                 while (!UART0.checkAndClearEvent(UART::Event::TX_Ready));
}

void p8(u8 data) {
    UART0.write('0' + (data >>   4)); while (!UART0.checkAndClearEvent(UART::Event::TX_Ready));
    UART0.write('0' + (data & 0x0F)); while (!UART0.checkAndClearEvent(UART::Event::TX_Ready));
    UART0.write(' ');                 while (!UART0.checkAndClearEvent(UART::Event::TX_Ready));
}
