#pragma once

#include <cstdint>
#include "ARK/System/CommunicationProtocols/I2c/I2c.hpp"

namespace Modules {

    #define BMP280_SUCCESS           0
    #define BMP280_ERR_COMM         -1
    #define BMP280_ERR_CHIP_ID      -2
    #define BMP280_ERR_NULL_PTR     -3
    #define BMP280_ERR_INVALID_CFG  -4

    #define BMP280_ADDR_0           0x76
    #define BMP280_ADDR_1           0x77

    enum class BMP280_Oversampling {
        SKIP = 0,
        X1   = 1,
        X2   = 2,
        X4   = 3,
        X8   = 4,
        X16  = 5
    };

    enum class BMP280_Filter {
        OFF = 0,
        X2  = 1,
        X4  = 2,
        X8  = 3,
        X16 = 4
    };

    enum class BMP280_Mode {
        SLEEP  = 0,
        FORCED = 1,
        NORMAL = 3
    };

    enum class BMP280_Standby {
        MS_0_5   = 0,
        MS_62_5  = 1,
        MS_125   = 2,
        MS_250   = 3,
        MS_500   = 4,
        MS_1000  = 5,
        MS_2000  = 6,
        MS_4000  = 7
    };

    class BMP280 {
    public:
        BMP280(ARK::I2C* i2c, uint8_t address = BMP280_ADDR_0);
        
        int Init();
        int SetConfig(BMP280_Oversampling temp_osrs, BMP280_Oversampling press_osrs,
                      BMP280_Filter filter, BMP280_Mode mode);
        int SetStandby(BMP280_Standby standby_time);
        int ReadTemperature(float* temperature);
        int ReadPressure(float* pressure);
        int ReadAltitude(float sea_level_hPa, float* altitude);
        int ForceMeasurement();
        int IsMeasuring(uint8_t* measuring);
        int Reset();

    private:
        ARK::I2C* _i2c;
        uint8_t _address;
        
        uint16_t dig_T1;
        int16_t  dig_T2;
        int16_t  dig_T3;
        uint16_t dig_P1;
        int16_t  dig_P2;
        int16_t  dig_P3;
        int16_t  dig_P4;
        int16_t  dig_P5;
        int16_t  dig_P6;
        int16_t  dig_P7;
        int16_t  dig_P8;
        int16_t  dig_P9;
        int32_t  t_fine;

        int WaitMeasuring(uint32_t timeout_ms);
    };
}
