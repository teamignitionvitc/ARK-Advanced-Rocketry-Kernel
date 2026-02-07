#pragma once

#include <cstdint>
#include "ARK/System/CommunicationProtocols/I2c/I2c.hpp"

namespace Modules {

    #define MPU9250_SUCCESS       0
    #define MPU9250_ERR_COMM     -1
    #define MPU9250_ERR_NULL_PTR -2

    #define MPU9250_ADDR_0       0x68
    #define MPU9250_ADDR_1       0x69

    enum class MPU_AccelScale {
        G_2  = 0,
        G_4  = 1,
        G_8  = 2,
        G_16 = 3
    };

    enum class MPU_GyroScale {
        DPS_250  = 0,
        DPS_500  = 1,
        DPS_1000 = 2,
        DPS_2000 = 3
    };

    struct MPU_Data {
        float accel[3];
        float gyro[3];
        float temp;
    };

    class MPU9250 {
    public:
        MPU9250(ARK::I2C* i2c, uint8_t address = MPU9250_ADDR_0);
        
        int Init();
        int SetAccelScale(MPU_AccelScale scale);
        int SetGyroScale(MPU_GyroScale scale);
        int ReadData(MPU_Data* data);
        int ReadAccel(float* accel);
        int ReadGyro(float* gyro);
        int ReadTemp(float* temp);
        int Reset();

    private:
        ARK::I2C* _i2c;
        uint8_t _address;
        float _accel_scale;
        float _gyro_scale;
    };
}
