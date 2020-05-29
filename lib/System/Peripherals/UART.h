#pragma once

#include <lib/Names.h>
#include <lib/Instructions.h>
#include <lib/System/PinNumber.h>
#include <lib/System/Peripherals/Helpers.h>

namespace System {
namespace Peripherals {

/// Definitions

    struct UART
    {
        NRF_UART_Type *const instance;

        struct Config
        {
            enum struct Baudrate : u32 {
                Baud_1200    = UART_BAUDRATE_BAUDRATE_Baud1200,
                Baud_2400    = UART_BAUDRATE_BAUDRATE_Baud2400,
                Baud_4800    = UART_BAUDRATE_BAUDRATE_Baud4800,
                Baud_9600    = UART_BAUDRATE_BAUDRATE_Baud9600,
                Baud_14400   = UART_BAUDRATE_BAUDRATE_Baud14400,
                Baud_19200   = UART_BAUDRATE_BAUDRATE_Baud19200,
                Baud_28800   = UART_BAUDRATE_BAUDRATE_Baud28800,
                Baud_31250   = UART_BAUDRATE_BAUDRATE_Baud31250,
                Baud_38400   = UART_BAUDRATE_BAUDRATE_Baud38400,
                Baud_56000   = UART_BAUDRATE_BAUDRATE_Baud56000,
                Baud_57600   = UART_BAUDRATE_BAUDRATE_Baud57600,
                Baud_76800   = UART_BAUDRATE_BAUDRATE_Baud76800,
                Baud_115200  = UART_BAUDRATE_BAUDRATE_Baud115200,
                Baud_230400  = UART_BAUDRATE_BAUDRATE_Baud230400,
                Baud_250000  = UART_BAUDRATE_BAUDRATE_Baud250000,
                Baud_460800  = UART_BAUDRATE_BAUDRATE_Baud460800,
                Baud_921600  = UART_BAUDRATE_BAUDRATE_Baud921600,
                Baud_1000000 = UART_BAUDRATE_BAUDRATE_Baud1M,
            };
            enum struct Parity : u32 {
                Disabled = UART_CONFIG_PARITY_Excluded, // No parity bit
                Enabled  = UART_CONFIG_PARITY_Included, // Automatic parity bit (even parity)
            };
            enum struct FlowControl : u32 {
                Disabled = UART_CONFIG_HWFC_Disabled, // Disable hardware flow control
                Enabled  = UART_CONFIG_HWFC_Enabled,  // Enable  hardware flow control
            };

            Baudrate          baudrate;
            Parity            parity;
            FlowControl       flowControl;
            System::PinNumber pinRXD;
            System::PinNumber pinTXD;
            System::PinNumber pinCTS;
            System::PinNumber pinRTS;
        };
        static constexpr Config defaultConfig = {
            .baudrate    = Config::Baudrate::Baud_250000,
            .parity      = Config::Parity::Disabled,
            .flowControl = Config::FlowControl::Disabled,
            .pinRXD      = System::PinNumber::None,
            .pinTXD      = System::PinNumber::None,
            .pinCTS      = System::PinNumber::None,
            .pinRTS      = System::PinNumber::None,
        };

        enum struct Task : u32 {
            Start_RX = offsetof(NRF_UART_Type, TASKS_STARTRX), // Start reception
            Stop_RX  = offsetof(NRF_UART_Type, TASKS_STOPRX),  // Stop  reception
            Start_TX = offsetof(NRF_UART_Type, TASKS_STARTTX), // Start transmission
            Stop_TX  = offsetof(NRF_UART_Type, TASKS_STOPTX),  // Stop  transmission (immediately)
            Suspend  = offsetof(NRF_UART_Type, TASKS_SUSPEND), // Suspend when complete
        };
        enum struct Event : u32 {
            CTS        = offsetof(NRF_UART_Type, EVENTS_CTS),    // CTS line activated
            NCTS       = offsetof(NRF_UART_Type, EVENTS_NCTS),   // CTS line deactivated
            RX_Ready   = offsetof(NRF_UART_Type, EVENTS_RXDRDY), // Data ready in RXD
            TX_Ready   = offsetof(NRF_UART_Type, EVENTS_TXDRDY), // Data sent from TXD
            Error      = offsetof(NRF_UART_Type, EVENTS_ERROR),  // Error detected
            RX_Timeout = offsetof(NRF_UART_Type, EVENTS_RXTO),   // Receiver timeout
        };
        enum struct Interrupt : u32 {
            None       = 0UL,
            CTS        = UART_INTENSET_CTS_Msk,
            NCTS       = UART_INTENSET_NCTS_Msk,
            RX_Ready   = UART_INTENSET_RXDRDY_Msk,
            TX_Ready   = UART_INTENSET_TXDRDY_Msk,
            Error      = UART_INTENSET_ERROR_Msk,
            RX_Timeout = UART_INTENSET_RXTO_Msk,
            All        = ~0UL,
        };
        enum struct Shortcut : u32 {};

        void init(Config config = defaultConfig);
        void enable();
        void disable();
        bool isEnabled();
        void shutdown();

        void applyConfig    (Config config);
        void setBaudrate    (Config::Baudrate    baudrate);
        void setParity      (Config::Parity      parity);
        void setFlowControl (Config::FlowControl flowControl);
        void setPinRXD      (System::PinNumber   pinRXD);
        void setPinTXD      (System::PinNumber   pinTXD);
        void setPinCTS      (System::PinNumber   pinCTS);
        void setPinRTS      (System::PinNumber   pinRTS);

        void write(u8 data);
        u8   read();

        void       triggerTask             (Task      task);
        u32            getTaskAddress      (Task      task);
        void         clearEvent            (Event     event);
        bool         checkEvent            (Event     event);
        bool checkAndClearEvent            (Event     event);
        u32            getEventAddress     (Event     event);
        u32            getInterrupts       ();
        void        enableInterrupts       (Interrupt interrupts);
        void       disableInterrupts       (Interrupt interrupts);
        bool            isInterruptEnabled (Interrupt interrupt);
        // void           setShortcuts        (Shortcut  shortcuts);
        // u32            getShortcuts        ();
        // void        enableShortcuts        (Shortcut  shortcuts);
        // void       disableShortcuts        (Shortcut  shortcuts);
        // bool            isShortcutEnabled  (Shortcut  shortcut);
    };

    UART UART0 {NRF_UART0};

    u32 operator|(UART::Interrupt a, UART::Interrupt b) { return (u32)a | (u32)b; }
    u32 operator|(UART::Shortcut  a, UART::Shortcut  b) { return (u32)a | (u32)b; }

/// Implementation

    void UART::init(Config config)
    {
        applyConfig(config);
    }
    void UART::enable()
    {
        instance->ENABLE = UART_ENABLE_ENABLE_Enabled << UART_ENABLE_ENABLE_Pos;
    }
    void UART::disable()
    {
        instance->ENABLE = UART_ENABLE_ENABLE_Disabled << UART_ENABLE_ENABLE_Pos;
    }
    bool UART::isEnabled()
    {
        return instance->ENABLE == UART_ENABLE_ENABLE_Enabled << UART_ENABLE_ENABLE_Pos;
    }
    void UART::shutdown()
    {
        disable();
        applyConfig(defaultConfig);
    }

    void UART::applyConfig(Config config)
    {
        setBaudrate    (config.baudrate);
        setParity      (config.parity);
        setFlowControl (config.flowControl);
        setPinRXD      (config.pinRXD);
        setPinTXD      (config.pinTXD);
        setPinCTS      (config.pinCTS);
        setPinRTS      (config.pinRTS);
    }
    void UART::setBaudrate(Config::Baudrate baudrate)
    {
        instance->BAUDRATE = (u32)baudrate;
    }
    void UART::setParity(Config::Parity parity)
    {
        instance->CONFIG = BFI(instance->CONFIG, UART_CONFIG_PARITY_Pos, UART_CONFIG_PARITY_Msk, (u32)parity);
    }
    void UART::setFlowControl(Config::FlowControl flowControl)
    {
        instance->CONFIG = BFI(instance->CONFIG, UART_CONFIG_HWFC_Pos, UART_CONFIG_HWFC_Msk, (u32)flowControl);
    }
    void UART::setPinRXD (System::PinNumber pinRXD)
    {
        instance->PSELRXD = (u32)pinRXD;
    }
    void UART::setPinTXD (System::PinNumber pinTXD)
    {
        instance->PSELTXD = (u32)pinTXD;
    }
    void UART::setPinCTS (System::PinNumber pinCTS)
    {
        instance->PSELCTS = (u32)pinCTS;
    }
    void UART::setPinRTS (System::PinNumber pinRTS)
    {
        instance->PSELRTS = (u32)pinRTS;
    }

    void UART::write(u8 data)
    {
        instance->TXD = data;
    }
    u8   UART::read()
    {
        return instance->RXD;
    }

    void UART::triggerTask        (Task task)            {        Helpers::triggerTask        (instance, (u32)task);  }
    u32  UART::getTaskAddress     (Task task)            { return Helpers::getTaskAddress     (instance, (u32)task);  }
    void UART::clearEvent         (Event event)          {        Helpers::clearEvent         (instance, (u32)event); }
    bool UART::checkEvent         (Event event)          { return Helpers::checkEvent         (instance, (u32)event); }
    bool UART::checkAndClearEvent (Event event)          { return Helpers::checkAndClearEvent (instance, (u32)event); }
    u32  UART::getEventAddress    (Event event)          { return Helpers::getEventAddress    (instance, (u32)event); }
    u32  UART::getInterrupts      ()                     { return instance->INTENSET;                                 }
    void UART::enableInterrupts   (Interrupt interrupts) {        instance->INTENSET = (u32)interrupts;               }
    void UART::disableInterrupts  (Interrupt interrupts) {        instance->INTENCLR = (u32)interrupts;               }
    bool UART::isInterruptEnabled (Interrupt interrupt)  { return instance->INTENSET & (u32)interrupt;                }
    // void UART::setShortcuts       (Shortcut shortcuts)   {        instance->SHORTS   = (u32)shortcuts;                }
    // u32  UART::getShortcuts       ()                     { return instance->SHORTS;                                   }
    // void UART::enableShortcuts    (Shortcut shortcuts)   {        instance->SHORTS  |= (u32)shortcuts                 }
    // void UART::disableShortcuts   (Shortcut shortcuts)   {        instance->SHORTS  &= (u32)shortcuts                 }
    // bool UART::isShortcutEnabled  (Shortcut shortcut)    { return instance->SHORTS   & (u32)shortcuts                 }

}
}
