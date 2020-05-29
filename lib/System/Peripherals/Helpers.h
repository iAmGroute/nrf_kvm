#pragma once

#include <lib/Names.h>

namespace System {
namespace Peripherals {
namespace Helpers {

    void triggerTask(void *instance, u32 taskOffset)
    {
        *((volatile u32 *)((u8 *)instance + taskOffset)) = 0x1UL;
    }
    u32 getTaskAddress(void *instance, u32 taskOffset)
    {
        return (u32)((u8 *)instance + taskOffset);
    }
    void clearEvent(void *instance, u32 eventOffset)
    {
        *((volatile u32 *)((u8 *)instance + eventOffset)) = 0x0UL;
    }
    bool checkEvent(void *instance, u32 eventOffset)
    {
        return (bool)*(volatile u32 *)((u8 *)instance + eventOffset);
    }
    bool checkAndClearEvent(void *instance, u32 eventOffset)
    {
        bool result = checkEvent(instance, eventOffset);
        if (result) clearEvent(instance, eventOffset);
        return result;
    }
    u32 getEventAddress(void *instance, u32 eventOffset)
    {
        return (u32)((u8 *)instance + eventOffset);
    }

}
}
}
