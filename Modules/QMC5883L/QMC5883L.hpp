#pragma once

#include <cstdint>
#include "ARK/System/CommunicationProtocols/I2c/I2c.hpp"

namespace Modules {

    #define QMC5883L_SUCCESS       0
    #define QMC5883L_ERR_COMM     -1
    #define QMC5883L_ERR_NULL_PTR -2

    #define QMC5883L_ADDR         0x0D

    enum class QMC_Mode {
        STANDBY    = 0,
        CONTINUOUS = 1
    };

    enum class QMC_OutputRate {
        HZ_10  = 0,
        HZ_50  = 1,
        HZ_100 = 2,
        HZ_200 = 3
    };

    enum class QMC_Range {
        GAUSS_2 = 0,
        GAUSS_8 = 1
    };

    enum class QMC_Oversample {
        OSR_512 = 0,
        OSR_256 = 1,
        OSR_128 = 2,
        OSR_64  = 3
    };

    struct MAG_Data {
        int16_t x;
        int16_t y;
        int16_t z;
    };

    class QMC5883L {
    public:
        QMC5883L(ARK::I2C* i2c, uint8_t address = QMC5883L_ADDR);
        
        int Init();
        int SetConfig(QMC_Mode mode, QMC_OutputRate rate, QMC_Range range, QMC_Oversample osr);
        int ReadRaw(MAG_Data* data);
        int ReadRaw(int16_t* x, int16_t* y, int16_t* z);
        float GetHeading();
        int Reset();
        bool IsReady();

    private:
        ARK::I2C* _i2c;
        uint8_t _address;
    };
}
