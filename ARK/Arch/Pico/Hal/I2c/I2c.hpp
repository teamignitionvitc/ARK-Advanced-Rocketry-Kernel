#pragma once
#include "ARK/HAL/I2c/I2c.hpp"
#include "pico/stdlib.h"
#include "hardware/i2c.h"

namespace ARK {
    namespace HAL {
        namespace Pico {
            class I2C : public ARK::HAL::I2C {
            public:
                static constexpr uint32_t I2C_TIMEOUT_US = 100000; // 100ms timeout
                
                void Init(uint8_t port_num, uint8_t sda_pin, uint8_t scl_pin, uint32_t baudrate) override {
                    _inst = (port_num == 0) ? i2c0 : i2c1;
                    i2c_init(_inst, baudrate);
                    gpio_set_function(sda_pin, GPIO_FUNC_I2C);
                    gpio_set_function(scl_pin, GPIO_FUNC_I2C);
                    gpio_pull_up(sda_pin);
                    gpio_pull_up(scl_pin);
                }

                bool Write(uint8_t addr, uint8_t reg, uint8_t data) override {
                    uint8_t buf[] = {reg, data};
                    int result = i2c_write_timeout_us(_inst, addr, buf, 2, false, I2C_TIMEOUT_US);
                    return result == 2;
                }

                bool Write(uint8_t addr, uint8_t* data, size_t len) override {
                    int result = i2c_write_timeout_us(_inst, addr, data, len, false, I2C_TIMEOUT_US);
                    return result == (int)len;
                }

                bool Read(uint8_t addr, uint8_t reg, uint8_t* buffer, size_t len) override {
                    int result = i2c_write_timeout_us(_inst, addr, &reg, 1, true, I2C_TIMEOUT_US);
                    if (result != 1) return false;
                    result = i2c_read_timeout_us(_inst, addr, buffer, len, false, I2C_TIMEOUT_US);
                    return result == (int)len;
                }

                bool Read(uint8_t addr, uint8_t* buffer, size_t len) override {
                    int result = i2c_read_timeout_us(_inst, addr, buffer, len, false, I2C_TIMEOUT_US);
                    return result == (int)len;
                }

            private:
                i2c_inst_t* _inst = nullptr;
            };
        }
    }
}
