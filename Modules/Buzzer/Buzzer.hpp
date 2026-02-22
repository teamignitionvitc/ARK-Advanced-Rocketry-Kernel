#pragma once

#include <cstdint>

namespace Modules {

    class Buzzer {
    public:
        Buzzer(uint8_t pin);
        
        void Init();
        void On();
        void Off();
        void Toggle();
        void Beep(uint32_t duration_ms);
        void Tone(uint32_t frequency_hz);
        void NoTone();
        bool IsOn() const;

    private:
        uint8_t _pin;
        bool _state;
        uint32_t _slice;
        uint32_t _channel;
    };
}
