# Common ARK Sources
# All paths are relative to CMAKE_SOURCE_DIR

set(ARK_COMMON_SOURCES
    ARK/Ark.cpp
    ARK/ark.cpp                             # New unified API
    ARK/Kernel/Loops/SuperiorLoop.cpp
    ARK/Kernel/StateMachine/States.cpp
    
    # MicroKernels
    ARK/Kernel/MicroKernels/ModulesKernel/ModulesKernel.cpp
    ARK/Kernel/MicroKernels/SystemKernel/SystemKernel.cpp
    ARK/Kernel/MicroKernels/UserKernel/UserKernel.cpp
    
    # System Components
    ARK/System/FDIR/FDIR.cpp
    ARK/System/Storage/Storage.cpp
    ARK/System/Console/Console.cpp
    ARK/System/Time/Time.cpp
    ARK/System/Clock/Clock.cpp
    ARK/System/Power/Power.cpp
    ARK/System/CommunicationProtocols/Gpio/Gpio.cpp
    ARK/System/CommunicationProtocols/I2c/I2c.cpp
    ARK/System/CommunicationProtocols/Spi/Spi.cpp
    ARK/System/CommunicationProtocols/Uart/Uart.cpp
    ARK/System/Watchdog/Watchdog.cpp
    ARK/System/Peripherals/Pwm/Pwm.cpp
    ARK/System/Peripherals/Adc/Adc.cpp

    # Modules (conditionally compiled via #if guards)
    Modules/ModulesManager.cpp
    Modules/WS2812/WS2812.cpp
    Modules/BMP280/BMP280.cpp
    Modules/MPU9250/MPU9250.cpp
    Modules/Buzzer/Buzzer.cpp
    Modules/LED/LED.cpp

    # User Code
    User/UserCode.cpp
)

set(ARK_COMMON_INCLUDE_DIRS
    .
    ARK
    ARK/HAL
    ARK/Kernel/Loops
    ARK/Kernel/MicroKernels/ModulesKernel
    ARK/Kernel/MicroKernels/SystemKernel
    ARK/Kernel/MicroKernels/UserKernel
    ARK/Kernel/StateMachine
    Modules
    User
    ARK/System/CommunicationProtocols/Gpio
    ARK/System/CommunicationProtocols/I2c
    ARK/System/CommunicationProtocols/Spi
    ARK/System/CommunicationProtocols/Uart
    ARK/System/Peripherals/Pwm
    ARK/System/Peripherals/Adc
    ARK/System/Watchdog
    ARK/System/Power
    ARK/System/FDIR
)
