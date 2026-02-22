#ifndef ARK_MODULES_MANAGER_HPP
#define ARK_MODULES_MANAGER_HPP

#include "../Config.hpp"
// Conditionally include only enabled modules (zero overhead for disabled modules)
#if ENABLE_BMP280
#include "BMP280/BMP280.hpp"
#endif

#if ENABLE_MPU9250
#include "MPU9250/MPU9250.hpp"
#endif

#if ENABLE_L89
#include "L89/L89.hpp"
#endif

#if ENABLE_QMC5883L
#include "QMC5883L/QMC5883L.hpp"
#endif

#if ENABLE_NRF24
#include "NRF24/NRF24.hpp"
#endif

#if ENABLE_BUZZER
#include "Buzzer/Buzzer.hpp"
#endif

#if ENABLE_LED
#include "LED/LED.hpp"
#endif

namespace ARK {
namespace Modules {
    void Initialize();
    void Update();
    
    // Conditionally declare getters - disabled modules don't expose getters
#if ENABLE_BMP280
    ::Modules::BMP280& GetBMP();
#endif

#if ENABLE_MPU9250
    ::Modules::MPU9250& GetMPU();
#endif

#if ENABLE_QMC5883L
    ::Modules::QMC5883L& GetMag();
#endif

#if ENABLE_L89
    ::Modules::L89& GetGPS();
#endif

#if ENABLE_NRF24
    ::Modules::NRF24& GetRadio();
#endif

#if ENABLE_BUZZER
    ::Modules::Buzzer& GetBuzzer();
#endif

#if ENABLE_LED
    ::Modules::LED& GetLED();
#endif
}
}

#endif
