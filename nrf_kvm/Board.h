#pragma once

#include <lib/System/GPIO.h>

#define nrf_delay_us(us_time) nrfx_coredep_delay_us(us_time)

namespace Board {
    using namespace System;
    using namespace System::GPIO;

    Pin Pin_Switch {PinNumber::P0_17};

    Pin Pin_Channel[4] = {
        Pin {PinNumber::P0_14},
        Pin {PinNumber::P0_16},
        Pin {PinNumber::P0_15},
        Pin {PinNumber::P0_13}
    };

    u8 currentChannel = 0;

    void configPins()
    {
        Pin_Switch.setInput();
        Pin_Channel[0].low();
        Pin_Channel[1].low();
        Pin_Channel[2].low();
        Pin_Channel[3].low();
        Pin_Channel[0].setOutput();
        Pin_Channel[1].setOutput();
        Pin_Channel[2].setOutput();
        Pin_Channel[3].setOutput();
        Pin_Channel[0].setStrength(Strength::Standard_Low_Strong_High);
        Pin_Channel[1].setStrength(Strength::Standard_Low_Strong_High);
        Pin_Channel[2].setStrength(Strength::Standard_Low_Strong_High);
        Pin_Channel[3].setStrength(Strength::Standard_Low_Strong_High);
        Pin_Channel[0].high();
    }

    void setChannel(u8 index)
    {
        Pin_Channel[currentChannel].low();
        Pin_Channel[index].high();
        currentChannel = index;
    }
}
