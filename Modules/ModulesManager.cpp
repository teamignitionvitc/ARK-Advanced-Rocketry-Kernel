#include "ModulesManager.hpp"
#include "Config.hpp"
#include "ARK/HAL/Console/Console.hpp"

#if ENABLE_I2C
#include "ARK/System/CommunicationProtocols/I2c/I2c.hpp"
#endif

namespace ARK {
namespace Modules {

    // I2C bus - only instantiated if any I2C sensor is enabled
#if ENABLE_I2C
    static ARK::I2C i2c(I2C_INSTANCE, I2C_SDA_PIN, I2C_SCL_PIN);
#endif

    // Conditionally instantiate modules (disabled modules use ZERO RAM)
#if ENABLE_BMP280
    static ::Modules::BMP280 bmp(&i2c);
#endif

#if ENABLE_MPU9250
    static ::Modules::MPU9250 mpu(&i2c);
#endif

#if ENABLE_QMC5883L
    static ::Modules::QMC5883L mag(&i2c);
#endif

#if ENABLE_BUZZER
    static ::Modules::Buzzer buzzer(PIN_BUZZER);
#endif

#if ENABLE_LED
    static ::Modules::LED led(PIN_LED);
#endif

    void Initialize() {
        // Initialize LED first so we can see if things are working
#if ENABLE_LED
        led.Init();
        SystemConsole.Print("[MODULES] LED init OK (GPIO %d)", PIN_LED);
        led.On();
#endif
        
        // Initialize buzzer (non-blocking)
#if ENABLE_BUZZER
        buzzer.Init();
        SystemConsole.Print("[MODULES] Buzzer init OK (GPIO %d)", PIN_BUZZER);
#endif
        
        // Initialize I2C bus
#if ENABLE_I2C
        i2c.Begin(I2C_FREQUENCY);
        SystemConsole.Print("[MODULES] I2C%d init OK (SDA=%d, SCL=%d, %d Hz)", 
                           I2C_INSTANCE, I2C_SDA_PIN, I2C_SCL_PIN, I2C_FREQUENCY);
#endif
        
        // Initialize I2C sensors
#if ENABLE_BMP280
        int bmpResult = bmp.Init();
        if (bmpResult != 0) {
            SystemConsole.Print("[MODULES] BMP280 init FAILED: %d", bmpResult);
        } else {
            SystemConsole.Print("[MODULES] BMP280 init OK");
        }
#endif
        
#if ENABLE_MPU9250
        int mpuResult = mpu.Init();
        if (mpuResult != 0) {
            SystemConsole.Print("[MODULES] MPU9250 init FAILED: %d", mpuResult);
        } else {
            SystemConsole.Print("[MODULES] MPU9250 init OK");
        }
#endif

#if ENABLE_QMC5883L
        int magResult = mag.Init();
        if (magResult != 0) {
            SystemConsole.Print("[MODULES] QMC5883L init FAILED: %d", magResult);
        } else {
            SystemConsole.Print("[MODULES] QMC5883L init OK");
        }
#endif

        SystemConsole.Print("[MODULES] Initialization complete");
    }

    void Update() {
        // Update modules that need periodic polling
    }

    // Conditionally define getters (disabled modules don't expose getters)
#if ENABLE_BMP280
    ::Modules::BMP280& GetBMP() {
        return bmp;
    }
#endif

#if ENABLE_MPU9250
    ::Modules::MPU9250& GetMPU() {
        return mpu;
    }
#endif

#if ENABLE_QMC5883L
    ::Modules::QMC5883L& GetMag() {
        return mag;
    }
#endif

#if ENABLE_BUZZER
    ::Modules::Buzzer& GetBuzzer() {
        return buzzer;
    }
#endif

#if ENABLE_LED
    ::Modules::LED& GetLED() {
        return led;
    }
#endif

}
}
