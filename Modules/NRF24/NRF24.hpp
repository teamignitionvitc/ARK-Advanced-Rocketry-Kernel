#pragma once

#include <cstdint>
#include <cstddef>
#include "ARK/System/CommunicationProtocols/Spi/Spi.hpp"
#include "ARK/System/CommunicationProtocols/Gpio/Gpio.hpp"

namespace Modules {

    #define NRF24_SUCCESS       0
    #define NRF24_ERR_COMM     -1
    #define NRF24_ERR_NULL_PTR -2
    #define NRF24_ERR_TIMEOUT  -3

    enum class NRF_DataRate {
        DR_250KBPS = 0,
        DR_1MBPS   = 1,
        DR_2MBPS   = 2
    };

    enum class NRF_Power {
        PA_MIN  = 0,
        PA_LOW  = 1,
        PA_HIGH = 2,
        PA_MAX  = 3
    };

    enum class NRF_CRCLength {
        CRC_DISABLED = 0,
        CRC_8        = 1,
        CRC_16       = 2
    };

    class NRF24 {
    public:
        NRF24(ARK::SPI* spi, ARK::GPIO* cs_gpio, ARK::GPIO* ce_gpio);
        
        int Init();
        int SetChannel(uint8_t channel);
        int SetDataRate(NRF_DataRate rate);
        int SetPower(NRF_Power power);
        int SetCRCLength(NRF_CRCLength length);
        int SetRetries(uint8_t delay, uint8_t count);
        int SetPayloadSize(uint8_t size);
        int SetTXAddress(const uint8_t* address, uint8_t len);
        int SetRXAddress(uint8_t pipe, const uint8_t* address, uint8_t len);
        int OpenWritingPipe(const uint8_t* address);
        int OpenReadingPipe(uint8_t pipe, const uint8_t* address);
        int StartListening();
        int StopListening();
        int Send(const uint8_t* data, size_t len);
        int Write(const uint8_t* data, size_t len);
        bool Available();
        int Read(uint8_t* data, size_t len);
        int FlushTX();
        int FlushRX();
        int PowerUp();
        int PowerDown();

    private:
        ARK::SPI* _spi;
        ARK::GPIO* _cs;
        ARK::GPIO* _ce;
        uint8_t _payload_size;
        bool _listening;

        uint8_t ReadRegister(uint8_t reg);
        int WriteRegister(uint8_t reg, uint8_t value);
        int WriteRegister(uint8_t reg, const uint8_t* data, uint8_t len);
        void CSLow();
        void CSHigh();
        void CELow();
        void CEHigh();
        void PulseCE();
    };
}
