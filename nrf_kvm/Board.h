#pragma once

#include <lib/System/GPIO.h>

#define nrf_delay_us(us_time) nrfx_coredep_delay_us(us_time)

namespace Board {
    using namespace System;
    using namespace System::GPIO;

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

    u8 currentChannel = 0;

    void configPS2Pin(Pin &pin)
    {
        pin.connectInputBuffer();
        pin.setStrength(Strength::Standard_Low_Open_High);
        pin.setOutput();
    }

    void configPins()
    {
        configPS2Pin(Pin_Keyboard_Clock);
        configPS2Pin(Pin_Keyboard_Data);
        configPS2Pin(Pin_Mouse_Clock);
        configPS2Pin(Pin_Mouse_Data);
        Pin_Switch.setInput();
        for (u8 i = 0; i < 4; i++) {
            Pin_Channel[i].low();
            Pin_Channel[i].setStrength(Strength::Standard_Low_Strong_High);
            Pin_Channel[i].setOutput();
        }
        Pin_Channel[0].high();
    }

    void setChannel(u8 index)
    {
        Pin_Channel[currentChannel].low();
        Pin_Channel[index].high();
        currentChannel = index;
    }
}
