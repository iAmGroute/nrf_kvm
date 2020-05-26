#pragma once

#include <lib/System/GPIO.h>

namespace Board {
    using namespace System;
    using namespace System::GPIO;

    Pin Pin_ChannelSwitch {PinNumber::P0_15};
    Pin Pin_ChannelId0    {PinNumber::P0_14};
    Pin Pin_ChannelId1    {PinNumber::P0_13};

    void configPins()
    {
        Pin_ChannelSwitch.low();
        Pin_ChannelSwitch.setOutput();
        Pin_ChannelId0.setInput();
        Pin_ChannelId1.setInput();
    }
}
