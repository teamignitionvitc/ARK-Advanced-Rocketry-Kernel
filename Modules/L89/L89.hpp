#pragma once

#include <cstdint>
#include <cstddef>
#include "ARK/System/CommunicationProtocols/Uart/Uart.hpp"

namespace Modules {

    #define L89_SUCCESS       0
    #define L89_ERR_COMM     -1
    #define L89_ERR_NULL_PTR -2
    #define L89_ERR_PARSE    -3

    struct GNSS_Data {
        float latitude;
        float longitude;
        float altitude;
        float speed;
        float course;
        uint8_t satellites;
        uint8_t fix_quality;
        bool valid;
    };

    class L89 {
    public:
        L89(ARK::UART* uart);
        
        int Init();
        int ReadRaw(char* buffer, size_t len);
        int ParseNMEA(const char* nmea, GNSS_Data* data);
        int Update(GNSS_Data* data);
        bool HasFix();

    private:
        ARK::UART* _uart;
        GNSS_Data _last_data;
        char _buffer[256];
        size_t _buf_index;

        float ParseCoordinate(const char* str, char dir);
        float ParseFloat(const char* str);
        int ParseInt(const char* str);
        bool ParseGGA(const char* sentence, GNSS_Data* data);
        bool ParseRMC(const char* sentence, GNSS_Data* data);
    };
}
