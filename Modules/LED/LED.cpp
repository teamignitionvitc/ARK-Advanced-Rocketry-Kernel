#include "LED.hpp"
#include "ARK/HAL/Time/Time.hpp"

#ifdef ARK_ARCH_PICO
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#endif

namespace Modules {

    LED::LED(uint8_t pin) : _pin(pin), _state(false), _slice(0), _channel(0), _pwm_mode(false) {}

    void LED::Init() {
#ifdef ARK_ARCH_PICO
        // Initialize as GPIO first (simple on/off)
        gpio_init(_pin);
        gpio_set_dir(_pin, GPIO_OUT);
        gpio_put(_pin, 0);
#endif
    }

    void LED::On() {
#ifdef ARK_ARCH_PICO
        if (_pwm_mode) {
            pwm_set_chan_level(_slice, _channel, 255);
        } else {
            gpio_put(_pin, 1);
        }
        _state = true;
#endif
    }

    void LED::Off() {
#ifdef ARK_ARCH_PICO
        if (_pwm_mode) {
            pwm_set_chan_level(_slice, _channel, 0);
        } else {
            gpio_put(_pin, 0);
        }
        _state = false;
#endif
    }

    void LED::Toggle() {
        if (_state) {
            Off();
        } else {
            On();
        }
    }

    void LED::Blink(uint32_t on_ms, uint32_t off_ms) {
        On();
        ARK::SystemTime.SleepUs(on_ms * 1000);
        Off();
        ARK::SystemTime.SleepUs(off_ms * 1000);
    }

    void LED::SetBrightness(uint8_t brightness) {
#ifdef ARK_ARCH_PICO
        // Switch to PWM mode if not already
        if (!_pwm_mode) {
            gpio_set_function(_pin, GPIO_FUNC_PWM);
            _slice = pwm_gpio_to_slice_num(_pin);
            _channel = pwm_gpio_to_channel(_pin);
            pwm_set_wrap(_slice, 255);
            pwm_set_enabled(_slice, true);
            _pwm_mode = true;
        }
        
        pwm_set_chan_level(_slice, _channel, brightness);
        _state = (brightness > 0);
#endif
    }

    bool LED::IsOn() const {
        return _state;
    }
}
