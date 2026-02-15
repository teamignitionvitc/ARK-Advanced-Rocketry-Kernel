#include "ARK/Ark.hpp"
#include "ARK/System/CommunicationProtocols/Gpio/Gpio.hpp"
#include "ARK/HAL/Time/Time.hpp"
#include "ARK/HAL/Console/Console.hpp"
#include "Modules/ModulesManager.hpp"

// #ifdef ARK_ARCH_PICO
// #include "Modules/WS2812/WS2812.hpp"
// #endif

// #ifdef ARK_ARCH_PICO
// #define PIN_NEOPIXEL 16
// Modules::WS2812* onboardLed = nullptr;
// #elif defined(ARK_ARCH_ESP32)
// #define PIN_LED 2
// ARK::GPIO espLed(PIN_LED);
// #endif

void UserSetup() {
// #ifdef ARK_ARCH_PICO
//     onboardLed = new Modules::WS2812(pio0, PIN_NEOPIXEL, 1);
// #elif defined(ARK_ARCH_ESP32)
//     espLed.Mode(ARK::HAL::PinMode::OUTPUT);
// #endif
}

void UserLoop() {

    // static bool state = false;
    // static uint64_t lastToggle = 0;
    
    // uint64_t currentTime = ARK::SystemTime.GetMicros();

    // if (currentTime - lastToggle >= 500000) {
    //     state = !state;
    //     float temperature = 0.0f;
    //     float pressure = 0.0f;
    //     Modules::MPU_Data mpuData;
    //     ARK::Modules::GetBMP().ReadTemperature(&temperature);
    //     ARK::Modules::GetBMP().ReadPressure(&pressure);
    //     ARK::Modules::GetMPU().ReadData(&mpuData);

         ARK::SystemConsole.Print("BMP280 - Temp:");
    //     ARK::SystemConsole.Print("MPU9250 - Accel: [%.2f, %.2f, %.2f] Gyro: [%.2f, %.2f, %.2f]\n",
    //         mpuData.accel[0], mpuData.accel[1], mpuData.accel[2],
    //         mpuData.gyro[0], mpuData.gyro[1], mpuData.gyro[2]);
        
    //     lastToggle = currentTime;
    // }
}