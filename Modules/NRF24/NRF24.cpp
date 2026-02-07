#include "NRF24.hpp"
#include "ARK/HAL/Time/Time.hpp"

namespace Modules {

    #define NRF24_REG_CONFIG      0x00
    #define NRF24_REG_EN_AA       0x01
    #define NRF24_REG_EN_RXADDR   0x02
    #define NRF24_REG_SETUP_AW    0x03
    #define NRF24_REG_SETUP_RETR  0x04
    #define NRF24_REG_RF_CH       0x05
    #define NRF24_REG_RF_SETUP    0x06
    #define NRF24_REG_STATUS      0x07
    #define NRF24_REG_RX_ADDR_P0  0x0A
    #define NRF24_REG_RX_ADDR_P1  0x0B
    #define NRF24_REG_TX_ADDR     0x10
    #define NRF24_REG_RX_PW_P0    0x11
    #define NRF24_REG_FIFO_STATUS 0x17
    #define NRF24_REG_DYNPD       0x1C
    #define NRF24_REG_FEATURE     0x1D

    #define NRF24_CMD_R_REGISTER    0x00
    #define NRF24_CMD_W_REGISTER    0x20
    #define NRF24_CMD_R_RX_PAYLOAD  0x61
    #define NRF24_CMD_W_TX_PAYLOAD  0xA0
    #define NRF24_CMD_FLUSH_TX      0xE1
    #define NRF24_CMD_FLUSH_RX      0xE2
    #define NRF24_CMD_NOP           0xFF

    #define NRF24_CONFIG_MASK_RX_DR  (1 << 6)
    #define NRF24_CONFIG_MASK_TX_DS  (1 << 5)
    #define NRF24_CONFIG_MASK_MAX_RT (1 << 4)
    #define NRF24_CONFIG_EN_CRC      (1 << 3)
    #define NRF24_CONFIG_CRCO        (1 << 2)
    #define NRF24_CONFIG_PWR_UP      (1 << 1)
    #define NRF24_CONFIG_PRIM_RX     (1 << 0)

    #define NRF24_STATUS_RX_DR       (1 << 6)
    #define NRF24_STATUS_TX_DS       (1 << 5)
    #define NRF24_STATUS_MAX_RT      (1 << 4)
    #define NRF24_STATUS_RX_P_NO     (0x07 << 1)
    #define NRF24_STATUS_TX_FULL     (1 << 0)

    #define NRF24_FIFO_STATUS_RX_EMPTY (1 << 0)

    NRF24::NRF24(ARK::SPI* spi, ARK::GPIO* cs_gpio, ARK::GPIO* ce_gpio)
        : _spi(spi), _cs(cs_gpio), _ce(ce_gpio), _payload_size(32), _listening(false) {}

    void NRF24::CSLow() {
        if (_cs) _cs->Set(false);
    }

    void NRF24::CSHigh() {
        if (_cs) _cs->Set(true);
    }

    void NRF24::CELow() {
        if (_ce) _ce->Set(false);
    }

    void NRF24::CEHigh() {
        if (_ce) _ce->Set(true);
    }

    void NRF24::PulseCE() {
        CEHigh();
        ARK::SystemTime.SleepUs(15);
        CELow();
    }

    uint8_t NRF24::ReadRegister(uint8_t reg) {
        uint8_t tx[2] = {(uint8_t)(NRF24_CMD_R_REGISTER | (reg & 0x1F)), 0xFF};
        uint8_t rx[2] = {0};
        
        CSLow();
        _spi->Transfer(tx, rx, 2);
        CSHigh();
        
        return rx[1];
    }

    int NRF24::WriteRegister(uint8_t reg, uint8_t value) {
        if (!_spi) return NRF24_ERR_NULL_PTR;

        uint8_t tx[2] = {(uint8_t)(NRF24_CMD_W_REGISTER | (reg & 0x1F)), value};
        
        CSLow();
        _spi->Write(tx, 2);
        CSHigh();
        
        return NRF24_SUCCESS;
    }

    int NRF24::WriteRegister(uint8_t reg, const uint8_t* data, uint8_t len) {
        if (!_spi || !data) return NRF24_ERR_NULL_PTR;

        uint8_t cmd = NRF24_CMD_W_REGISTER | (reg & 0x1F);
        
        CSLow();
        _spi->Write(&cmd, 1);
        _spi->Write((uint8_t*)data, len);
        CSHigh();
        
        return NRF24_SUCCESS;
    }

    int NRF24::Init() {
        if (!_spi || !_cs || !_ce) return NRF24_ERR_NULL_PTR;

        _cs->Mode(ARK::HAL::PinMode::OUTPUT);
        _ce->Mode(ARK::HAL::PinMode::OUTPUT);
        CSHigh();
        CELow();

        ARK::SystemTime.SleepUs(5000);

        WriteRegister(NRF24_REG_CONFIG, 0x0C);
        WriteRegister(NRF24_REG_EN_AA, 0x3F);
        WriteRegister(NRF24_REG_EN_RXADDR, 0x03);
        WriteRegister(NRF24_REG_SETUP_AW, 0x03);
        WriteRegister(NRF24_REG_SETUP_RETR, 0x03);
        WriteRegister(NRF24_REG_RF_CH, 76);
        WriteRegister(NRF24_REG_RF_SETUP, 0x06);
        WriteRegister(NRF24_REG_RX_PW_P0, _payload_size);
        WriteRegister(NRF24_REG_DYNPD, 0x00);
        WriteRegister(NRF24_REG_FEATURE, 0x00);

        FlushTX();
        FlushRX();

        WriteRegister(NRF24_REG_STATUS, NRF24_STATUS_RX_DR | NRF24_STATUS_TX_DS | NRF24_STATUS_MAX_RT);

        return NRF24_SUCCESS;
    }

    int NRF24::SetChannel(uint8_t channel) {
        if (channel > 125) channel = 125;
        return WriteRegister(NRF24_REG_RF_CH, channel);
    }

    int NRF24::SetDataRate(NRF_DataRate rate) {
        uint8_t setup = ReadRegister(NRF24_REG_RF_SETUP);
        setup &= ~0x28;

        switch (rate) {
            case NRF_DataRate::DR_250KBPS:
                setup |= 0x20;
                break;
            case NRF_DataRate::DR_1MBPS:
                break;
            case NRF_DataRate::DR_2MBPS:
                setup |= 0x08;
                break;
        }

        return WriteRegister(NRF24_REG_RF_SETUP, setup);
    }

    int NRF24::SetPower(NRF_Power power) {
        uint8_t setup = ReadRegister(NRF24_REG_RF_SETUP);
        setup &= ~0x06;
        setup |= ((uint8_t)power << 1);
        return WriteRegister(NRF24_REG_RF_SETUP, setup);
    }

    int NRF24::SetCRCLength(NRF_CRCLength length) {
        uint8_t config = ReadRegister(NRF24_REG_CONFIG);
        config &= ~(NRF24_CONFIG_EN_CRC | NRF24_CONFIG_CRCO);

        switch (length) {
            case NRF_CRCLength::CRC_8:
                config |= NRF24_CONFIG_EN_CRC;
                break;
            case NRF_CRCLength::CRC_16:
                config |= NRF24_CONFIG_EN_CRC | NRF24_CONFIG_CRCO;
                break;
            default:
                break;
        }

        return WriteRegister(NRF24_REG_CONFIG, config);
    }

    int NRF24::SetRetries(uint8_t delay, uint8_t count) {
        return WriteRegister(NRF24_REG_SETUP_RETR, ((delay & 0x0F) << 4) | (count & 0x0F));
    }

    int NRF24::SetPayloadSize(uint8_t size) {
        if (size > 32) size = 32;
        _payload_size = size;
        return WriteRegister(NRF24_REG_RX_PW_P0, size);
    }

    int NRF24::SetTXAddress(const uint8_t* address, uint8_t len) {
        if (len > 5) len = 5;
        return WriteRegister(NRF24_REG_TX_ADDR, address, len);
    }

    int NRF24::SetRXAddress(uint8_t pipe, const uint8_t* address, uint8_t len) {
        if (pipe > 5 || len > 5) return NRF24_ERR_COMM;
        return WriteRegister(NRF24_REG_RX_ADDR_P0 + pipe, address, pipe <= 1 ? len : 1);
    }

    int NRF24::OpenWritingPipe(const uint8_t* address) {
        SetTXAddress(address, 5);
        SetRXAddress(0, address, 5);
        WriteRegister(NRF24_REG_RX_PW_P0, _payload_size);
        return NRF24_SUCCESS;
    }

    int NRF24::OpenReadingPipe(uint8_t pipe, const uint8_t* address) {
        if (pipe > 5) return NRF24_ERR_COMM;
        
        SetRXAddress(pipe, address, 5);
        WriteRegister(NRF24_REG_RX_PW_P0 + pipe, _payload_size);
        
        uint8_t en_rxaddr = ReadRegister(NRF24_REG_EN_RXADDR);
        WriteRegister(NRF24_REG_EN_RXADDR, en_rxaddr | (1 << pipe));
        
        return NRF24_SUCCESS;
    }

    int NRF24::StartListening() {
        uint8_t config = ReadRegister(NRF24_REG_CONFIG);
        config |= NRF24_CONFIG_PWR_UP | NRF24_CONFIG_PRIM_RX;
        WriteRegister(NRF24_REG_CONFIG, config);
        
        WriteRegister(NRF24_REG_STATUS, NRF24_STATUS_RX_DR | NRF24_STATUS_TX_DS | NRF24_STATUS_MAX_RT);
        
        CEHigh();
        ARK::SystemTime.SleepUs(130);
        _listening = true;
        
        return NRF24_SUCCESS;
    }

    int NRF24::StopListening() {
        CELow();
        ARK::SystemTime.SleepUs(130);
        
        uint8_t config = ReadRegister(NRF24_REG_CONFIG);
        config &= ~NRF24_CONFIG_PRIM_RX;
        WriteRegister(NRF24_REG_CONFIG, config);
        
        _listening = false;
        return NRF24_SUCCESS;
    }

    int NRF24::Send(const uint8_t* data, size_t len) {
        return Write(data, len);
    }

    int NRF24::Write(const uint8_t* data, size_t len) {
        if (!_spi || !data) return NRF24_ERR_NULL_PTR;

        uint8_t config = ReadRegister(NRF24_REG_CONFIG);
        config |= NRF24_CONFIG_PWR_UP;
        config &= ~NRF24_CONFIG_PRIM_RX;
        WriteRegister(NRF24_REG_CONFIG, config);
        
        ARK::SystemTime.SleepUs(130);

        uint8_t cmd = NRF24_CMD_W_TX_PAYLOAD;
        CSLow();
        _spi->Write(&cmd, 1);
        _spi->Write((uint8_t*)data, len > _payload_size ? _payload_size : len);
        CSHigh();

        PulseCE();

        uint32_t timeout = 500;
        while (timeout--) {
            uint8_t status = ReadRegister(NRF24_REG_STATUS);
            if (status & (NRF24_STATUS_TX_DS | NRF24_STATUS_MAX_RT)) {
                WriteRegister(NRF24_REG_STATUS, NRF24_STATUS_TX_DS | NRF24_STATUS_MAX_RT);
                if (status & NRF24_STATUS_MAX_RT) {
                    FlushTX();
                    return NRF24_ERR_TIMEOUT;
                }
                return NRF24_SUCCESS;
            }
            ARK::SystemTime.SleepUs(10);
        }

        return NRF24_ERR_TIMEOUT;
    }

    bool NRF24::Available() {
        uint8_t fifo = ReadRegister(NRF24_REG_FIFO_STATUS);
        return !(fifo & NRF24_FIFO_STATUS_RX_EMPTY);
    }

    int NRF24::Read(uint8_t* data, size_t len) {
        if (!_spi || !data) return NRF24_ERR_NULL_PTR;

        uint8_t cmd = NRF24_CMD_R_RX_PAYLOAD;
        CSLow();
        _spi->Write(&cmd, 1);
        
        uint8_t* rx_buf = new uint8_t[len];
        uint8_t* tx_buf = new uint8_t[len];
        for (size_t i = 0; i < len; i++) tx_buf[i] = 0xFF;
        
        _spi->Transfer(tx_buf, rx_buf, len > _payload_size ? _payload_size : len);
        CSHigh();
        
        for (size_t i = 0; i < (len > _payload_size ? _payload_size : len); i++) {
            data[i] = rx_buf[i];
        }
        
        delete[] rx_buf;
        delete[] tx_buf;

        WriteRegister(NRF24_REG_STATUS, NRF24_STATUS_RX_DR);

        return NRF24_SUCCESS;
    }

    int NRF24::FlushTX() {
        uint8_t cmd = NRF24_CMD_FLUSH_TX;
        CSLow();
        _spi->Write(&cmd, 1);
        CSHigh();
        return NRF24_SUCCESS;
    }

    int NRF24::FlushRX() {
        uint8_t cmd = NRF24_CMD_FLUSH_RX;
        CSLow();
        _spi->Write(&cmd, 1);
        CSHigh();
        return NRF24_SUCCESS;
    }

    int NRF24::PowerUp() {
        uint8_t config = ReadRegister(NRF24_REG_CONFIG);
        config |= NRF24_CONFIG_PWR_UP;
        WriteRegister(NRF24_REG_CONFIG, config);
        ARK::SystemTime.SleepUs(1500);
        return NRF24_SUCCESS;
    }

    int NRF24::PowerDown() {
        CELow();
        uint8_t config = ReadRegister(NRF24_REG_CONFIG);
        config &= ~NRF24_CONFIG_PWR_UP;
        return WriteRegister(NRF24_REG_CONFIG, config);
    }
}
