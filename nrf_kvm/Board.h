#pragma once

#include "Common.h"
#include <lib/System/GPIO.h>
#include <lib/System/Peripherals/UART.h>
#include "PS2.h"

namespace Board {
    using namespace System;
    using namespace System::GPIO;
    using namespace System::Peripherals;

    Pin Pin_UART_RX        {PinNumber::P0_09};
    Pin Pin_UART_TX        {PinNumber::P0_10};
    Pin Pin_Keyboard_Clock {PinNumber::P0_29};
    Pin Pin_Keyboard_Data  {PinNumber::P0_30};
    Pin Pin_Mouse_Clock    {PinNumber::P0_11};
    Pin Pin_Mouse_Data     {PinNumber::P0_12};
    Pin Pin_Switch         {PinNumber::P0_17};
    Pin Pin_Channel[4] = {
        Pin {PinNumber::P0_14},
        Pin {PinNumber::P0_16},
        Pin {PinNumber::P0_15},
        Pin {PinNumber::P0_13}
    };

    Interface_PS2 keyboard { Pin_Keyboard_Clock, Pin_Keyboard_Data };
    Interface_PS2 mouse    { Pin_Mouse_Clock,    Pin_Mouse_Data };

    u8 currentChannel = 0;

    void _configPins()
    {
        Pin_UART_RX.setInput();
        Pin_UART_RX.connectInputBuffer();
        Pin_UART_TX.high();
        Pin_UART_TX.setOutput();
        Pin_Switch.setInput();
        Pin_Switch.connectInputBuffer();
        for (u8 i = 0; i < 4; i++) {
            Pin_Channel[i].low();
            Pin_Channel[i].setStrength(Strength::Standard_Low_Strong_High);
            Pin_Channel[i].setOutput();
        }
        Pin_Channel[0].high();
    }

    void init()
    {
        _configPins();
        UART0.init(UART::Config {
            .baudrate    = UART::Config::Baudrate::Baud_1000000,
            .parity      = UART::Config::Parity::Disabled,
            .flowControl = UART::Config::FlowControl::Disabled,
            .pinRXD      = Pin_UART_RX,
            .pinTXD      = Pin_UART_TX,
            .pinCTS      = PinNumber::None,
            .pinRTS      = PinNumber::None,
        });
        UART0.enable();
        UART0.triggerTask(UART::Task::Start_TX);
    }

    void setChannel(u8 index)
    {
        Pin_Channel[currentChannel].low();
        Pin_Channel[index].high();
        currentChannel = index;
    }
}
