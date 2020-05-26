#pragma once

#include <lib/Instructions.h>
#include <lib/System/PinNumber.h>

namespace System {
namespace GPIO {

    enum struct State : bool {
        Low  = false,
        High = true,
    };

    enum struct Mode : bool {
        Input = false,
        Output = true,
    };

    struct Output
    {
        virtual void low() = 0;
        virtual void high() = 0;
        virtual void write(State State) = 0;
    };

    struct Input
    {
        virtual bool isLow() = 0;
        virtual bool isHigh() = 0;
        virtual State read() = 0;
    };

    struct Bidirectional
    {
        virtual void setInput() = 0;
        virtual void setOutput() = 0;
        virtual void setMode(Mode mode) = 0;
    };

    struct Pin : Output, Input, Bidirectional
    {
        const System::PinNumber pinNumber;
        const u8 position;
        NRF_GPIO_Type *const instance;

        static u8 getPosition(System::PinNumber pinNumber)
        {
            u8 num = (u8)pinNumber;
            // if (num < System::PinCount) return num % 32;
            // else return ~0U;
            assert(num < System::PinCount);
            return num % 32;
        }

        static NRF_GPIO_Type *getInstance(System::PinNumber pinNumber)
        {
            #ifdef NRF_GPIO
                return NRF_GPIO;
            #else
                u8 num = (u8)pinNumber;
                return num < P0_PIN_NUM ? NRF_P0 : NRF_P1;
            #endif
        }

        Pin(System::PinNumber pinNumber)
            : pinNumber(pinNumber), position(getPosition(pinNumber)), instance(getInstance(pinNumber))
        { }

        operator System::PinNumber()
        {
            return pinNumber;
        }

        void low()
        {
            instance->OUTCLR = 1UL << position;
        }
        void high()
        {
            instance->OUTSET = 1UL << position;
        }
        void write(State state)
        {
            state == State::Low ? low() : high();
        }
        bool isLow()
        {
            return read() == State::Low;
        }
        bool isHigh()
        {
            return read() == State::High;
        }
        State read()
        {
            return VAL(instance->IN, position) == 0 ? State::Low : State::High;
        }
        void setInputFast()
        {
            instance->DIRCLR = 1UL << position;
        }
        void setOutputFast()
        {
            instance->DIRSET = 1UL << position;
        }
        void connectInputBuffer()
        {
            instance->PIN_CNF[position] = SBI(instance->PIN_CNF[position], GPIO_PIN_CNF_INPUT_Pos);
        }
        void disconnectInputBuffer()
        {
            instance->PIN_CNF[position] = CBI(instance->PIN_CNF[position], GPIO_PIN_CNF_INPUT_Pos);
        }
        void setInput()
        {
            u32 temp = instance->PIN_CNF[position];
            temp = CBI(temp, GPIO_PIN_CNF_DIR_Pos);
            temp = SBI(temp, GPIO_PIN_CNF_INPUT_Pos);
            instance->PIN_CNF[position] = temp;
        }
        void setOutput()
        {
            u32 temp = instance->PIN_CNF[position];
            temp = SBI(temp, GPIO_PIN_CNF_DIR_Pos);
            temp = CBI(temp, GPIO_PIN_CNF_INPUT_Pos);
            instance->PIN_CNF[position] = temp;
        }
        void setMode(Mode mode)
        {
            mode == Mode::Input ? setInput() : setOutput();
        }
    };

}
}
