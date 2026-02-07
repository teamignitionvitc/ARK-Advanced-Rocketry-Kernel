#include "L89.hpp"
#include <cstring>
#include <cstdlib>

namespace Modules {

    L89::L89(ARK::UART* uart) : _uart(uart), _buf_index(0) {
        _last_data = {0};
        memset(_buffer, 0, sizeof(_buffer));
    }

    int L89::Init() {
        if (!_uart) return L89_ERR_NULL_PTR;
        return L89_SUCCESS;
    }

    int L89::ReadRaw(char* buffer, size_t len) {
        if (!_uart || !buffer) return L89_ERR_NULL_PTR;

        size_t bytes_read = _uart->Read((uint8_t*)buffer, len - 1, 100);
        buffer[bytes_read] = '\0';
        return (int)bytes_read;
    }

    float L89::ParseCoordinate(const char* str, char dir) {
        if (!str || strlen(str) < 4) return 0.0f;

        float raw = (float)atof(str);
        int degrees = (int)(raw / 100);
        float minutes = raw - (degrees * 100);
        float result = degrees + (minutes / 60.0f);

        if (dir == 'S' || dir == 'W') {
            result = -result;
        }

        return result;
    }

    float L89::ParseFloat(const char* str) {
        if (!str || strlen(str) == 0) return 0.0f;
        return (float)atof(str);
    }

    int L89::ParseInt(const char* str) {
        if (!str || strlen(str) == 0) return 0;
        return atoi(str);
    }

    bool L89::ParseGGA(const char* sentence, GNSS_Data* data) {
        char copy[128];
        strncpy(copy, sentence, sizeof(copy) - 1);
        copy[sizeof(copy) - 1] = '\0';

        char* token = strtok(copy, ",");
        int field = 0;
        
        char lat_str[16] = {0};
        char lat_dir = 'N';
        char lon_str[16] = {0};
        char lon_dir = 'E';

        while (token != nullptr && field < 15) {
            switch (field) {
                case 2:
                    strncpy(lat_str, token, sizeof(lat_str) - 1);
                    break;
                case 3:
                    lat_dir = token[0];
                    break;
                case 4:
                    strncpy(lon_str, token, sizeof(lon_str) - 1);
                    break;
                case 5:
                    lon_dir = token[0];
                    break;
                case 6:
                    data->fix_quality = (uint8_t)ParseInt(token);
                    break;
                case 7:
                    data->satellites = (uint8_t)ParseInt(token);
                    break;
                case 9:
                    data->altitude = ParseFloat(token);
                    break;
            }
            token = strtok(nullptr, ",");
            field++;
        }

        data->latitude = ParseCoordinate(lat_str, lat_dir);
        data->longitude = ParseCoordinate(lon_str, lon_dir);
        data->valid = (data->fix_quality > 0);

        return data->valid;
    }

    bool L89::ParseRMC(const char* sentence, GNSS_Data* data) {
        char copy[128];
        strncpy(copy, sentence, sizeof(copy) - 1);
        copy[sizeof(copy) - 1] = '\0';

        char* token = strtok(copy, ",");
        int field = 0;
        
        char lat_str[16] = {0};
        char lat_dir = 'N';
        char lon_str[16] = {0};
        char lon_dir = 'E';
        char status = 'V';

        while (token != nullptr && field < 13) {
            switch (field) {
                case 2:
                    status = token[0];
                    break;
                case 3:
                    strncpy(lat_str, token, sizeof(lat_str) - 1);
                    break;
                case 4:
                    lat_dir = token[0];
                    break;
                case 5:
                    strncpy(lon_str, token, sizeof(lon_str) - 1);
                    break;
                case 6:
                    lon_dir = token[0];
                    break;
                case 7:
                    data->speed = ParseFloat(token) * 1.852f;
                    break;
                case 8:
                    data->course = ParseFloat(token);
                    break;
            }
            token = strtok(nullptr, ",");
            field++;
        }

        data->latitude = ParseCoordinate(lat_str, lat_dir);
        data->longitude = ParseCoordinate(lon_str, lon_dir);
        data->valid = (status == 'A');

        return data->valid;
    }

    int L89::ParseNMEA(const char* nmea, GNSS_Data* data) {
        if (!nmea || !data) return L89_ERR_NULL_PTR;

        if (strncmp(nmea, "$GPGGA", 6) == 0 || strncmp(nmea, "$GNGGA", 6) == 0) {
            if (ParseGGA(nmea, data)) {
                return L89_SUCCESS;
            }
        } else if (strncmp(nmea, "$GPRMC", 6) == 0 || strncmp(nmea, "$GNRMC", 6) == 0) {
            if (ParseRMC(nmea, data)) {
                return L89_SUCCESS;
            }
        }

        return L89_ERR_PARSE;
    }

    int L89::Update(GNSS_Data* data) {
        if (!_uart || !data) return L89_ERR_NULL_PTR;

        uint8_t byte;
        while (_uart->Read(&byte, 1, 10) > 0) {
            if (byte == '$') {
                _buf_index = 0;
            }

            if (_buf_index < sizeof(_buffer) - 1) {
                _buffer[_buf_index++] = (char)byte;
                _buffer[_buf_index] = '\0';
            }

            if (byte == '\n' || byte == '\r') {
                if (_buf_index > 6) {
                    int ret = ParseNMEA(_buffer, &_last_data);
                    if (ret == L89_SUCCESS) {
                        *data = _last_data;
                        _buf_index = 0;
                        return L89_SUCCESS;
                    }
                }
                _buf_index = 0;
            }
        }

        *data = _last_data;
        return L89_SUCCESS;
    }

    bool L89::HasFix() {
        return _last_data.valid && _last_data.fix_quality > 0;
    }
}
