#include "Buzzer.hpp"
#include "ARK/HAL/Time/Time.hpp"

#ifdef ARK_ARCH_PICO
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"
#endif

namespace Modules {

    Buzzer::Buzzer(uint8_t pin) : _pin(pin), _state(false), _slice(0), _channel(0) {}

    void Buzzer::Init() {
#ifdef ARK_ARCH_PICO
        gpio_set_function(_pin, GPIO_FUNC_PWM);
        _slice = pwm_gpio_to_slice_num(_pin);
        _channel = pwm_gpio_to_channel(_pin);
        pwm_set_wrap(_slice, 255);
        pwm_set_chan_level(_slice, _channel, 0);
        pwm_set_enabled(_slice, true);
#endif
    }

    void Buzzer::On() {
#ifdef ARK_ARCH_PICO
        pwm_set_chan_level(_slice, _channel, 128);  // 50% duty cycle
        _state = true;
#endif
    }

    void Buzzer::Off() {
#ifdef ARK_ARCH_PICO
        pwm_set_chan_level(_slice, _channel, 0);
        _state = false;
#endif
    }

    void Buzzer::Toggle() {
        if (_state) {
            Off();
        } else {
            On();
        }
    }

    void Buzzer::Beep(uint32_t duration_ms) {
        On();
        ARK::SystemTime.SleepUs(duration_ms * 1000);
        Off();
    }

    void Buzzer::Tone(uint32_t frequency_hz) {
#ifdef ARK_ARCH_PICO
        if (frequency_hz == 0) {
            NoTone();
            return;
        }
        
        uint32_t clock_freq = clock_get_hz(clk_sys);
        uint32_t divider = clock_freq / (frequency_hz * 256);
        if (divider < 1) divider = 1;
        if (divider > 255) divider = 255;
        
        pwm_set_clkdiv(_slice, (float)divider);
        pwm_set_wrap(_slice, 255);
        pwm_set_chan_level(_slice, _channel, 128);  // 50% duty cycle
        _state = true;
#endif
    }

    void Buzzer::NoTone() {
#ifdef ARK_ARCH_PICO
        pwm_set_chan_level(_slice, _channel, 0);
        _state = false;
#endif
    }

    bool Buzzer::IsOn() const {
        return _state;
    }
}
