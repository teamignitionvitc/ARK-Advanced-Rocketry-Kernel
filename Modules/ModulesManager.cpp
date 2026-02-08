#include "ModulesManager.hpp"
#include "Config.hpp"
#include "ARK/System/CommunicationProtocols/I2c/I2c.hpp"

namespace ARK {
namespace Modules {

    static ARK::I2C i2c(0, 4, 5);
    static ::Modules::BMP280 bmp(&i2c);
    static ::Modules::MPU9250 mpu(&i2c);

    void Initialize() {
        i2c.Begin(100000);
        bmp.Init();
        mpu.Init();
    }

    void Update() {
    }

    ::Modules::BMP280& GetBMP() {
        return bmp;
    }

    ::Modules::MPU9250& GetMPU() {
        return mpu;
    }

}
}
