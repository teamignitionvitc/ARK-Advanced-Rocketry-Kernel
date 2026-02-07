#include "MPU9250.hpp"
#include "ARK/HAL/Time/Time.hpp"

namespace Modules {

    #define MPU9250_REG_PWR_MGMT_1   0x6B
    #define MPU9250_REG_CONFIG       0x1A
    #define MPU9250_REG_GYRO_CONFIG  0x1B
    #define MPU9250_REG_ACCEL_CONFIG 0x1C
    #define MPU9250_REG_ACCEL_XOUT_H 0x3B
    #define MPU9250_REG_TEMP_OUT_H   0x41
    #define MPU9250_REG_GYRO_XOUT_H  0x43
    #define MPU9250_REG_WHO_AM_I     0x75

    MPU9250::MPU9250(ARK::I2C* i2c, uint8_t address) 
        : _i2c(i2c), _address(address), _accel_scale(16384.0f), _gyro_scale(131.0f) {}

    int MPU9250::Init() {
        if (!_i2c) return MPU9250_ERR_NULL_PTR;

        if (!_i2c->Write(_address, MPU9250_REG_PWR_MGMT_1, 0x80)) {
            return MPU9250_ERR_COMM;
        }
        ARK::SystemTime.SleepUs(100000);

        if (!_i2c->Write(_address, MPU9250_REG_PWR_MGMT_1, 0x00)) {
            return MPU9250_ERR_COMM;
        }
        ARK::SystemTime.SleepUs(10000);

        if (!_i2c->Write(_address, MPU9250_REG_CONFIG, 0x03)) {
            return MPU9250_ERR_COMM;
        }

        SetAccelScale(MPU_AccelScale::G_2);
        SetGyroScale(MPU_GyroScale::DPS_250);

        return MPU9250_SUCCESS;
    }

    int MPU9250::SetAccelScale(MPU_AccelScale scale) {
        if (!_i2c) return MPU9250_ERR_NULL_PTR;

        if (!_i2c->Write(_address, MPU9250_REG_ACCEL_CONFIG, (uint8_t)scale << 3)) {
            return MPU9250_ERR_COMM;
        }

        switch (scale) {
            case MPU_AccelScale::G_2:  _accel_scale = 16384.0f; break;
            case MPU_AccelScale::G_4:  _accel_scale = 8192.0f;  break;
            case MPU_AccelScale::G_8:  _accel_scale = 4096.0f;  break;
            case MPU_AccelScale::G_16: _accel_scale = 2048.0f;  break;
        }

        return MPU9250_SUCCESS;
    }

    int MPU9250::SetGyroScale(MPU_GyroScale scale) {
        if (!_i2c) return MPU9250_ERR_NULL_PTR;

        if (!_i2c->Write(_address, MPU9250_REG_GYRO_CONFIG, (uint8_t)scale << 3)) {
            return MPU9250_ERR_COMM;
        }

        switch (scale) {
            case MPU_GyroScale::DPS_250:  _gyro_scale = 131.0f;  break;
            case MPU_GyroScale::DPS_500:  _gyro_scale = 65.5f;   break;
            case MPU_GyroScale::DPS_1000: _gyro_scale = 32.8f;   break;
            case MPU_GyroScale::DPS_2000: _gyro_scale = 16.4f;   break;
        }

        return MPU9250_SUCCESS;
    }

    int MPU9250::ReadData(MPU_Data* data) {
        if (!_i2c || !data) return MPU9250_ERR_NULL_PTR;

        uint8_t buf[14];
        if (!_i2c->Read(_address, MPU9250_REG_ACCEL_XOUT_H, buf, 14)) {
            return MPU9250_ERR_COMM;
        }

        data->accel[0] = (int16_t)(buf[0] << 8 | buf[1]) / _accel_scale;
        data->accel[1] = (int16_t)(buf[2] << 8 | buf[3]) / _accel_scale;
        data->accel[2] = (int16_t)(buf[4] << 8 | buf[5]) / _accel_scale;
        data->temp = ((int16_t)(buf[6] << 8 | buf[7]) / 333.87f) + 21.0f;
        data->gyro[0] = (int16_t)(buf[8] << 8 | buf[9]) / _gyro_scale;
        data->gyro[1] = (int16_t)(buf[10] << 8 | buf[11]) / _gyro_scale;
        data->gyro[2] = (int16_t)(buf[12] << 8 | buf[13]) / _gyro_scale;

        return MPU9250_SUCCESS;
    }

    int MPU9250::ReadAccel(float* accel) {
        if (!_i2c || !accel) return MPU9250_ERR_NULL_PTR;

        uint8_t buf[6];
        if (!_i2c->Read(_address, MPU9250_REG_ACCEL_XOUT_H, buf, 6)) {
            return MPU9250_ERR_COMM;
        }

        accel[0] = (int16_t)(buf[0] << 8 | buf[1]) / _accel_scale;
        accel[1] = (int16_t)(buf[2] << 8 | buf[3]) / _accel_scale;
        accel[2] = (int16_t)(buf[4] << 8 | buf[5]) / _accel_scale;

        return MPU9250_SUCCESS;
    }

    int MPU9250::ReadGyro(float* gyro) {
        if (!_i2c || !gyro) return MPU9250_ERR_NULL_PTR;

        uint8_t buf[6];
        if (!_i2c->Read(_address, MPU9250_REG_GYRO_XOUT_H, buf, 6)) {
            return MPU9250_ERR_COMM;
        }

        gyro[0] = (int16_t)(buf[0] << 8 | buf[1]) / _gyro_scale;
        gyro[1] = (int16_t)(buf[2] << 8 | buf[3]) / _gyro_scale;
        gyro[2] = (int16_t)(buf[4] << 8 | buf[5]) / _gyro_scale;

        return MPU9250_SUCCESS;
    }

    int MPU9250::ReadTemp(float* temp) {
        if (!_i2c || !temp) return MPU9250_ERR_NULL_PTR;

        uint8_t buf[2];
        if (!_i2c->Read(_address, MPU9250_REG_TEMP_OUT_H, buf, 2)) {
            return MPU9250_ERR_COMM;
        }

        *temp = ((int16_t)(buf[0] << 8 | buf[1]) / 333.87f) + 21.0f;

        return MPU9250_SUCCESS;
    }

    int MPU9250::Reset() {
        if (!_i2c) return MPU9250_ERR_NULL_PTR;

        if (!_i2c->Write(_address, MPU9250_REG_PWR_MGMT_1, 0x80)) {
            return MPU9250_ERR_COMM;
        }
        ARK::SystemTime.SleepUs(100000);

        return MPU9250_SUCCESS;
    }
}
