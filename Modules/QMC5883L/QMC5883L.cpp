#include "QMC5883L.hpp"
#include "ARK/HAL/Time/Time.hpp"
#include <cmath>

namespace Modules {

    #define QMC5883L_REG_DATA_OUT_X_L 0x00
    #define QMC5883L_REG_STATUS       0x06
    #define QMC5883L_REG_CONTROL_1    0x09
    #define QMC5883L_REG_CONTROL_2    0x0A
    #define QMC5883L_REG_SET_RESET    0x0B

    #define QMC5883L_STATUS_DRDY      0x01

    QMC5883L::QMC5883L(ARK::I2C* i2c, uint8_t address) : _i2c(i2c), _address(address) {}

    int QMC5883L::Init() {
        if (!_i2c) return QMC5883L_ERR_NULL_PTR;

        if (!_i2c->Write(_address, QMC5883L_REG_SET_RESET, 0x01)) {
            return QMC5883L_ERR_COMM;
        }
        ARK::SystemTime.SleepUs(1000);

        int ret = SetConfig(QMC_Mode::CONTINUOUS, QMC_OutputRate::HZ_200, 
                            QMC_Range::GAUSS_8, QMC_Oversample::OSR_512);
        if (ret != QMC5883L_SUCCESS) return ret;

        return QMC5883L_SUCCESS;
    }

    int QMC5883L::SetConfig(QMC_Mode mode, QMC_OutputRate rate, QMC_Range range, QMC_Oversample osr) {
        if (!_i2c) return QMC5883L_ERR_NULL_PTR;

        uint8_t ctrl1 = ((uint8_t)osr << 6) | ((uint8_t)range << 4) | 
                        ((uint8_t)rate << 2) | (uint8_t)mode;

        if (!_i2c->Write(_address, QMC5883L_REG_CONTROL_1, ctrl1)) {
            return QMC5883L_ERR_COMM;
        }

        return QMC5883L_SUCCESS;
    }

    int QMC5883L::ReadRaw(MAG_Data* data) {
        if (!_i2c || !data) return QMC5883L_ERR_NULL_PTR;

        uint8_t buf[6];
        if (!_i2c->Read(_address, QMC5883L_REG_DATA_OUT_X_L, buf, 6)) {
            return QMC5883L_ERR_COMM;
        }

        data->x = (int16_t)(buf[1] << 8 | buf[0]);
        data->y = (int16_t)(buf[3] << 8 | buf[2]);
        data->z = (int16_t)(buf[5] << 8 | buf[4]);

        return QMC5883L_SUCCESS;
    }

    int QMC5883L::ReadRaw(int16_t* x, int16_t* y, int16_t* z) {
        if (!_i2c || !x || !y || !z) return QMC5883L_ERR_NULL_PTR;

        MAG_Data data;
        int ret = ReadRaw(&data);
        if (ret != QMC5883L_SUCCESS) return ret;

        *x = data.x;
        *y = data.y;
        *z = data.z;

        return QMC5883L_SUCCESS;
    }

    float QMC5883L::GetHeading() {
        MAG_Data data;
        if (ReadRaw(&data) != QMC5883L_SUCCESS) {
            return 0.0f;
        }

        float heading = atan2f((float)data.y, (float)data.x) * 180.0f / 3.14159265358979f;
        if (heading < 0) {
            heading += 360.0f;
        }

        return heading;
    }

    int QMC5883L::Reset() {
        if (!_i2c) return QMC5883L_ERR_NULL_PTR;

        if (!_i2c->Write(_address, QMC5883L_REG_CONTROL_2, 0x80)) {
            return QMC5883L_ERR_COMM;
        }
        ARK::SystemTime.SleepUs(1000);

        return QMC5883L_SUCCESS;
    }

    bool QMC5883L::IsReady() {
        if (!_i2c) return false;

        uint8_t status;
        if (!_i2c->Read(_address, QMC5883L_REG_STATUS, &status, 1)) {
            return false;
        }

        return (status & QMC5883L_STATUS_DRDY) != 0;
    }
}
