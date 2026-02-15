#include "BMP280.hpp"
#include "ARK/HAL/Time/Time.hpp"
#include <cmath>

namespace Modules {

    #define BMP280_REG_CHIP_ID      0xD0
    #define BMP280_REG_RESET        0xE0
    #define BMP280_REG_STATUS       0xF3
    #define BMP280_REG_DIG_T1       0x88
    #define BMP280_REG_CTRL_MEAS    0xF4
    #define BMP280_REG_CONFIG       0xF5
    #define BMP280_REG_PRESS_MSB    0xF7
    #define BMP280_REG_TEMP_MSB     0xFA
    #define BMP280_CHIP_ID          0x58
    #define BME280_CHIP_ID          0x60
    #define BMP280_RESET_VALUE      0xB6
    #define BMP280_CALIBRATION_SIZE 24
    #define BMP280_STATUS_MEASURING (1 << 3)

    BMP280::BMP280(ARK::I2C* i2c, uint8_t address) : _i2c(i2c), _address(address) {}

    int BMP280::Init() {
        if (!_i2c) return BMP280_ERR_NULL_PTR;

        uint8_t chip_id;
        if (!_i2c->Read(_address, BMP280_REG_CHIP_ID, &chip_id, 1)) {
            return BMP280_ERR_COMM;
        }

        if (chip_id != BMP280_CHIP_ID && chip_id != BME280_CHIP_ID) {
            return BMP280_ERR_CHIP_ID;
        }

        uint8_t calib[BMP280_CALIBRATION_SIZE];
        if (!_i2c->Read(_address, BMP280_REG_DIG_T1, calib, BMP280_CALIBRATION_SIZE)) {
            return BMP280_ERR_COMM;
        }

        dig_T1 = (uint16_t)((calib[1] << 8) | calib[0]);
        dig_T2 = (int16_t)((calib[3] << 8) | calib[2]);
        dig_T3 = (int16_t)((calib[5] << 8) | calib[4]);
        dig_P1 = (uint16_t)((calib[7] << 8) | calib[6]);
        dig_P2 = (int16_t)((calib[9] << 8) | calib[8]);
        dig_P3 = (int16_t)((calib[11] << 8) | calib[10]);
        dig_P4 = (int16_t)((calib[13] << 8) | calib[12]);
        dig_P5 = (int16_t)((calib[15] << 8) | calib[14]);
        dig_P6 = (int16_t)((calib[17] << 8) | calib[16]);
        dig_P7 = (int16_t)((calib[19] << 8) | calib[18]);
        dig_P8 = (int16_t)((calib[21] << 8) | calib[20]);
        dig_P9 = (int16_t)((calib[23] << 8) | calib[22]);

        int ret = Reset();
        if (ret != BMP280_SUCCESS) return ret;

        ret = SetConfig(BMP280_Oversampling::X2, BMP280_Oversampling::X16,
                        BMP280_Filter::X4, BMP280_Mode::NORMAL);
        if (ret != BMP280_SUCCESS) return ret;

        ret = SetStandby(BMP280_Standby::MS_62_5);
        if (ret != BMP280_SUCCESS) return ret;

        return BMP280_SUCCESS;
    }

    int BMP280::SetConfig(BMP280_Oversampling temp_osrs, BMP280_Oversampling press_osrs,
                          BMP280_Filter filter, BMP280_Mode mode) {
        if (!_i2c) return BMP280_ERR_NULL_PTR;

        uint8_t ctrl = ((uint8_t)temp_osrs << 5) | ((uint8_t)press_osrs << 2) | (uint8_t)mode;

        uint8_t config;
        if (!_i2c->Read(_address, BMP280_REG_CONFIG, &config, 1)) {
            return BMP280_ERR_COMM;
        }

        config = (config & 0xE3) | ((uint8_t)filter << 2);

        if (!_i2c->Write(_address, BMP280_REG_CTRL_MEAS, ctrl)) {
            return BMP280_ERR_COMM;
        }

        if (!_i2c->Write(_address, BMP280_REG_CONFIG, config)) {
            return BMP280_ERR_COMM;
        }

        return BMP280_SUCCESS;
    }

    int BMP280::SetStandby(BMP280_Standby standby_time) {
        if (!_i2c) return BMP280_ERR_NULL_PTR;

        uint8_t config;
        if (!_i2c->Read(_address, BMP280_REG_CONFIG, &config, 1)) {
            return BMP280_ERR_COMM;
        }

        config = (config & 0x1F) | ((uint8_t)standby_time << 5);

        if (!_i2c->Write(_address, BMP280_REG_CONFIG, config)) {
            return BMP280_ERR_COMM;
        }

        return BMP280_SUCCESS;
    }

    int BMP280::WaitMeasuring(uint32_t timeout_ms) {
        uint64_t start = ARK::SystemTime.GetMicros();
        uint8_t measuring;

        do {
            if (!_i2c->Read(_address, BMP280_REG_STATUS, &measuring, 1)) {
                return BMP280_ERR_COMM;
            }

            if (!(measuring & BMP280_STATUS_MEASURING)) {
                return BMP280_SUCCESS;
            }

            ARK::SystemTime.SleepUs(1000);
        } while ((ARK::SystemTime.GetMicros() - start) < (uint64_t)timeout_ms * 1000);

        return BMP280_ERR_COMM;
    }

    int BMP280::ReadTemperature(float* temperature) {
        if (!_i2c || !temperature) return BMP280_ERR_NULL_PTR;

        WaitMeasuring(50);

        uint8_t buf[3];
        if (!_i2c->Read(_address, BMP280_REG_TEMP_MSB, buf, 3)) {
            return BMP280_ERR_COMM;
        }

        int32_t adc_T = ((int32_t)buf[0] << 12) | ((int32_t)buf[1] << 4) | (buf[2] >> 4);

        int32_t var1 = ((((adc_T >> 3) - ((int32_t)dig_T1 << 1))) * dig_T2) >> 11;
        int32_t var2 = (((((adc_T >> 4) - dig_T1) * ((adc_T >> 4) - dig_T1)) >> 12) * dig_T3) >> 14;

        t_fine = var1 + var2;

        *temperature = ((t_fine * 5 + 128) >> 8) / 100.0f;
        return BMP280_SUCCESS;
    }

    int BMP280::ReadPressure(float* pressure) {
        if (!_i2c || !pressure) return BMP280_ERR_NULL_PTR;

        float temp;
        int ret = ReadTemperature(&temp);
        if (ret != BMP280_SUCCESS) return ret;

        WaitMeasuring(50);

        uint8_t buf[3];
        if (!_i2c->Read(_address, BMP280_REG_PRESS_MSB, buf, 3)) {
            return BMP280_ERR_COMM;
        }

        int32_t adc_P = ((int32_t)buf[0] << 12) | ((int32_t)buf[1] << 4) | (buf[2] >> 4);

        int64_t var1 = (int64_t)t_fine - 128000;
        int64_t var2 = var1 * var1 * dig_P6;
        var2 += (var1 * dig_P5) << 17;
        var2 += ((int64_t)dig_P4) << 35;

        var1 = ((var1 * var1 * dig_P3) >> 8) + ((var1 * dig_P2) << 12);
        var1 = (((int64_t)1 << 47) + var1) * dig_P1 >> 33;

        if (var1 == 0) {
            *pressure = 0.0f;
            return BMP280_SUCCESS;
        }

        int64_t p = 1048576 - adc_P;
        p = (((p << 31) - var2) * 3125) / var1;

        var1 = (dig_P9 * (p >> 13) * (p >> 13)) >> 25;
        var2 = (dig_P8 * p) >> 19;

        p = ((p + var1 + var2) >> 8) + ((int64_t)dig_P7 << 4);
        *pressure = (float)p / 256.0f;

        return BMP280_SUCCESS;
    }

    int BMP280::ReadAltitude(float sea_level_hPa, float* altitude) {
        if (!_i2c || !altitude) return BMP280_ERR_NULL_PTR;

        float pressure_Pa;
        int ret = ReadPressure(&pressure_Pa);
        if (ret != BMP280_SUCCESS) return ret;

        float pressure_hPa = pressure_Pa / 100.0f;
        *altitude = 44330.0f * (1.0f - powf(pressure_hPa / sea_level_hPa, 0.1903f));
        return BMP280_SUCCESS;
    }

    int BMP280::ForceMeasurement() {
        if (!_i2c) return BMP280_ERR_NULL_PTR;

        uint8_t ctrl;
        if (!_i2c->Read(_address, BMP280_REG_CTRL_MEAS, &ctrl, 1)) {
            return BMP280_ERR_COMM;
        }

        ctrl = (ctrl & 0xFC) | (uint8_t)BMP280_Mode::FORCED;
        if (!_i2c->Write(_address, BMP280_REG_CTRL_MEAS, ctrl)) {
            return BMP280_ERR_COMM;
        }

        return BMP280_SUCCESS;
    }

    int BMP280::IsMeasuring(uint8_t* measuring) {
        if (!_i2c || !measuring) return BMP280_ERR_NULL_PTR;

        uint8_t status;
        if (!_i2c->Read(_address, BMP280_REG_STATUS, &status, 1)) {
            return BMP280_ERR_COMM;
        }

        *measuring = (status & BMP280_STATUS_MEASURING) ? 1 : 0;
        return BMP280_SUCCESS;
    }

    int BMP280::Reset() {
        if (!_i2c) return BMP280_ERR_NULL_PTR;

        if (!_i2c->Write(_address, BMP280_REG_RESET, BMP280_RESET_VALUE)) {
            return BMP280_ERR_COMM;
        }

        ARK::SystemTime.SleepUs(2000);
        return BMP280_SUCCESS;
    }
}
