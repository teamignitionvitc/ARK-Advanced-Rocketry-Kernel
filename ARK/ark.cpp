/**
 * @file ark.cpp
 * @brief ARK Framework - Unified API Implementation
 */

#include "ark.h"
#include "HAL/Console/Console.hpp"
#include "HAL/Time/Time.hpp"
#include "System/CommunicationProtocols/Gpio/Gpio.hpp"
#include "../Modules/ModulesManager.hpp"
#include "Kernel/StateMachine/States.hpp"
#include <cstdarg>
#include <cmath>

// Global Ark instance
Ark ark;

// Pyro channel GPIO (static, initialized on first use)
static ARK::GPIO* pyroDrogue = nullptr;
static ARK::GPIO* pyroMain = nullptr;

static void initPyroChannels() {
    if (pyroDrogue == nullptr) {
        pyroDrogue = new ARK::GPIO(PIN_PYRO_DROGUE);
        pyroDrogue->Mode(ARK::HAL::PinMode::OUTPUT);
        pyroDrogue->Set(false);
    }
    if (pyroMain == nullptr) {
        pyroMain = new ARK::GPIO(PIN_PYRO_MAIN);
        pyroMain->Mode(ARK::HAL::PinMode::OUTPUT);
        pyroMain->Set(false);
    }
}

// =========================================================================
// Initialization
// =========================================================================

void Ark::begin() {
    _startTime = ARK::SystemTime.GetMillis();
    
    // Initialize pyro channels (safe, outputs low)
    initPyroChannels();
    
    // Initialize modules
    ARK::Modules::Initialize();
    
    // Set initial state
    state = FlightState::IDLE;
    FlightManager.SetState(FlightState::IDLE);
    
#if ENABLE_BMP280
    // Calibrate ground level
    calibrateAltimeter();
#endif
    
    ARK::SystemConsole.Print("[ARK] System initialized");
}

void Ark::update() {
    // Update state from state manager
    state = FlightManager.GetState();
    
    // Run state machine tick
    FlightManager.Tick();
    
    // Update sensors and derived values
    updateSensors();
    updateVelocity();
    
    // Track max altitude during flight
#if ENABLE_BMP280
    if (isFlying()) {
        float alt = altitude();
        if (alt > _maxAltitude) {
            _maxAltitude = alt;
        }
    }
#endif
}

void Ark::updateState() {
    state = FlightManager.GetState();
}

void Ark::updateSensors() {
    // Sensor updates are handled by ModulesKernel
    // This function updates derived values
}

void Ark::updateVelocity() {
    uint32_t now = ARK::SystemTime.GetMillis();
    uint32_t dt = now - _lastVelocityUpdate;
    
    if (dt >= 20) {  // Update at ~50Hz
#if ENABLE_BMP280
        float currentAlt = altitude();
        float dt_sec = dt / 1000.0f;
        _velocity = (currentAlt - _lastAltitude) / dt_sec;
        _lastAltitude = currentAlt;
#endif
        _lastVelocityUpdate = now;
    }
}

// =========================================================================
// State Control
// =========================================================================

void Ark::arm() {
    if (state == FlightState::IDLE) {
        FlightManager.SetState(FlightState::ARMED);
        state = FlightState::ARMED;
        ARK::SystemConsole.Print("[ARK] System ARMED");
#if ENABLE_BUZZER
        buzzer.beep(3);
#endif
    }
}

void Ark::disarm() {
    if (state == FlightState::ARMED) {
        FlightManager.SetState(FlightState::IDLE);
        state = FlightState::IDLE;
        ARK::SystemConsole.Print("[ARK] System DISARMED");
#if ENABLE_BUZZER
        buzzer.beep(1);
#endif
    }
}

// =========================================================================
// Sensor Data
// =========================================================================

#if ENABLE_BMP280
float Ark::altitude() {
    return ARK::Modules::GetBMP().ReadAltitude() - _groundAltitude;
}

float Ark::pressure() {
    return ARK::Modules::GetBMP().ReadPressure();
}

float Ark::temperature() {
    return ARK::Modules::GetBMP().ReadTemperature();
}

float Ark::maxAltitude() {
    return _maxAltitude;
}

void Ark::calibrateAltimeter() {
    // Average multiple readings for ground level
    float sum = 0;
    const int samples = 10;
    for (int i = 0; i < samples; i++) {
        sum += ARK::Modules::GetBMP().ReadAltitude();
        ARK::SystemTime.DelayMs(10);
    }
    _groundAltitude = sum / samples;
    _lastAltitude = 0;
    _maxAltitude = 0;
    ARK::SystemConsole.Print("[ARK] Ground altitude: %.2f m", _groundAltitude);
}
#endif

#if ENABLE_MPU9250
float Ark::acceleration() {
    float x = accelX();
    float y = accelY();
    float z = accelZ();
    return sqrtf(x*x + y*y + z*z);
}

float Ark::accelX() {
    return ARK::Modules::GetMPU().ReadAccelX();
}

float Ark::accelY() {
    return ARK::Modules::GetMPU().ReadAccelY();
}

float Ark::accelZ() {
    return ARK::Modules::GetMPU().ReadAccelZ();
}

float Ark::gyroX() {
    return ARK::Modules::GetMPU().ReadGyroX();
}

float Ark::gyroY() {
    return ARK::Modules::GetMPU().ReadGyroY();
}

float Ark::gyroZ() {
    return ARK::Modules::GetMPU().ReadGyroZ();
}
#endif

float Ark::velocity() {
    return _velocity;
}

// =========================================================================
// Recovery System
// =========================================================================

void Ark::deployDrogue() {
    if (!_drogueDeployed) {
        _drogueDeployed = true;
        // Fire drogue pyro channel
        if (pyroDrogue) {
            pyroDrogue->Set(true);
            ARK::SystemTime.DelayMs(100);  // Fire duration
            pyroDrogue->Set(false);
        }
        ARK::SystemConsole.Print("[ARK] DROGUE DEPLOYED");
#if ENABLE_BUZZER
        buzzer.beep(2);
#endif
    }
}

void Ark::deployMain() {
    if (!_mainDeployed) {
        _mainDeployed = true;
        // Fire main pyro channel
        if (pyroMain) {
            pyroMain->Set(true);
            ARK::SystemTime.DelayMs(100);  // Fire duration
            pyroMain->Set(false);
        }
        ARK::SystemConsole.Print("[ARK] MAIN DEPLOYED");
#if ENABLE_BUZZER
        buzzer.beep(3);
#endif
    }
}

// =========================================================================
// LED API
// =========================================================================

#if ENABLE_LED
void Ark::LED_API::on() {
    ARK::Modules::GetLED().On();
}

void Ark::LED_API::off() {
    ARK::Modules::GetLED().Off();
}

void Ark::LED_API::toggle() {
    ARK::Modules::GetLED().Toggle();
}

void Ark::LED_API::blink(uint32_t interval_ms) {
    static uint32_t lastToggle = 0;
    uint32_t now = ARK::SystemTime.GetMillis();
    if (now - lastToggle >= interval_ms) {
        ARK::Modules::GetLED().Toggle();
        lastToggle = now;
    }
}
#endif

// =========================================================================
// Buzzer API
// =========================================================================

#if ENABLE_BUZZER
void Ark::Buzzer_API::beep(uint8_t count) {
    for (uint8_t i = 0; i < count; i++) {
        ARK::Modules::GetBuzzer().On();
        ARK::SystemTime.DelayMs(100);
        ARK::Modules::GetBuzzer().Off();
        if (i < count - 1) {
            ARK::SystemTime.DelayMs(100);
        }
    }
}

void Ark::Buzzer_API::tone(uint16_t frequency_hz, uint32_t duration_ms) {
    ARK::Modules::GetBuzzer().SetFrequency(frequency_hz);
    ARK::Modules::GetBuzzer().On();
    if (duration_ms > 0) {
        ARK::SystemTime.DelayMs(duration_ms);
        ARK::Modules::GetBuzzer().Off();
    }
}

void Ark::Buzzer_API::stop() {
    ARK::Modules::GetBuzzer().Off();
}
#endif

// =========================================================================
// Logging
// =========================================================================

void Ark::log(const char* message) {
    ARK::SystemConsole.Print("[USER] %s", message);
}

void Ark::logf(const char* format, ...) {
    char buffer[128];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    ARK::SystemConsole.Print("[USER] %s", buffer);
}

// =========================================================================
// System Info
// =========================================================================

uint32_t Ark::uptime() const {
    return ARK::SystemTime.GetMillis() - _startTime;
}

uint32_t Ark::stateTime() const {
    // This would need state entry time tracking
    return 0;  // TODO: Implement with state manager
}
