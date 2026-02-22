#pragma once

#include <cstdint>

namespace Modules {

    class LED {
    public:
        LED(uint8_t pin);
        
        void Init();
        void On();
        void Off();
        void Toggle();
        void Blink(uint32_t on_ms, uint32_t off_ms);
        void SetBrightness(uint8_t brightness);  // 0-255, uses PWM
        bool IsOn() const;

    private:
        uint8_t _pin;
        bool _state;
        uint32_t _slice;
        uint32_t _channel;
        bool _pwm_mode;
    };
}
