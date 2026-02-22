cmake_minimum_required(VERSION 3.16)

# 1. Architecture Detection
set(ARCH_TARGET "Auto" CACHE STRING "Target architecture (PICO, ESP32, SIL)")

if(ARCH_TARGET STREQUAL "Auto")
    if(DEFINED ENV{IDF_PATH})
        set(ARCH_TARGET "ESP32")
    else()
        set(ARCH_TARGET "PICO")
    endif()
endif()

message(STATUS "ARK Architecture Target: ${ARCH_TARGET}")

# 2. Universal Source Files (Defined in helper)
include(ARK/sources.cmake)

if(ARCH_TARGET STREQUAL "ESP32")
    # Register the ARK ESP32 component (which acts as 'main')
    set(EXTRA_COMPONENT_DIRS "ARK/Arch/Esp32")
    
    set(SDKCONFIG "${CMAKE_SOURCE_DIR}/Configuration/sdkconfig")
    include($ENV{IDF_PATH}/tools/cmake/project.cmake)
    project(ARK_Avionics)
    
    # Common ESP32 Build Properties
    idf_build_set_property(COMPILE_OPTIONS "-ffunction-sections" "-fdata-sections" APPEND)
    idf_build_set_property(LINK_OPTIONS "-Wl,--gc-sections" APPEND)

elseif(ARCH_TARGET STREQUAL "PICO")
    project(ARK_Avionics C CXX ASM)
    include(pico_sdk_import.cmake)
    pico_sdk_init()
    
    add_compile_options(-ffunction-sections -fdata-sections)
    add_link_options(-Wl,--gc-sections)
    add_definitions(-DARK_ARCH_PICO)
    
    set(PICO_SOURCES
        ${ARK_COMMON_SOURCES}
        "ARK/Arch/Pico/Entry.cpp"
        # HAL Implementations (cpp files)
        "ARK/Arch/Pico/Hal/Clock/Clock.cpp"
        "ARK/Arch/Pico/Hal/Console/Console.cpp"
        "ARK/Arch/Pico/Hal/Flash/Flash.cpp"
        "ARK/Arch/Pico/Hal/Time/Time.cpp"
        # Note: Gpio, I2c, Spi, Uart are currently header-only implementations in Pico/Hal
    )
    
    add_executable(ARK_Avionics ${PICO_SOURCES})
    set_target_properties(ARK_Avionics PROPERTIES OUTPUT_NAME "ARK_Avionics" SUFFIX ".elf")

    # PIO Generation
    pico_generate_pio_header(ARK_Avionics ${CMAKE_SOURCE_DIR}/Modules/WS2812/ws2812.pio)
    
    target_include_directories(ARK_Avionics PUBLIC 
        ${ARK_COMMON_INCLUDE_DIRS}
        # Specialized Pico Include Paths
        ARK/Arch/Pico/Hal
        ARK/Arch/Pico/Hal/Time
        ARK/Arch/Pico/Hal/Clock
        ARK/Arch/Pico/Hal/Console
        ARK/Arch/Pico/Hal/Flash 
        ARK/Arch/Pico/Hal/Gpio
        ARK/Arch/Pico/Hal/I2c
        ARK/Arch/Pico/Hal/Spi
        ARK/Arch/Pico/Hal/Uart
        ARK/Arch/Pico/Hal/Watchdog
        ARK/Arch/Pico/Hal/Pwm
        ARK/Arch/Pico/Hal/Adc
        ${CMAKE_CURRENT_BINARY_DIR}
    )
    
    target_link_libraries(ARK_Avionics 
        pico_stdlib 
        pico_stdio_usb
        hardware_clocks 
        hardware_vreg
        hardware_i2c
        hardware_spi
        hardware_uart
        hardware_flash
        hardware_adc
        hardware_pio
        hardware_pwm
        hardware_watchdog
        hardware_gpio
    )
    
    pico_enable_stdio_usb(ARK_Avionics 1)
    pico_enable_stdio_uart(ARK_Avionics 0)
    pico_add_extra_outputs(ARK_Avionics)

elseif(ARCH_TARGET STREQUAL "SIL")
    project(ARK_Avionics CXX)
    add_definitions(-DARK_ARCH_SIL)
    
    set(SIL_SOURCES
        ${ARK_COMMON_SOURCES}
        "ARK/Arch/SIL/Entry.cpp"
        "ARK/Arch/SIL/Hal/Clock/Clock.cpp" 
        "ARK/Arch/SIL/Hal/Console/Console.cpp" 
        "ARK/Arch/SIL/Hal/Flash/Flash.cpp"
        "ARK/Arch/SIL/Hal/Time/Time.cpp"
        "ARK/Arch/SIL/Hal/Gpio/Gpio.cpp"
        "ARK/Arch/SIL/Hal/I2c/I2c.cpp"
        "ARK/Arch/SIL/Hal/Spi/Spi.cpp"
        "ARK/Arch/SIL/Hal/Uart/Uart.cpp"
        "ARK/Arch/SIL/Hal/Watchdog/Watchdog.cpp"
        "ARK/Arch/SIL/Hal/Pwm/Pwm.cpp"
        "ARK/Arch/SIL/Hal/Adc/Adc.cpp"
    )
    
    add_executable(ARK_Avionics_SIL ${SIL_SOURCES})

    target_include_directories(ARK_Avionics_SIL PUBLIC 
        ${ARK_COMMON_INCLUDE_DIRS}
        ARK/Arch/SIL/Hal
        ARK/Arch/SIL/Hal/Time
        ARK/Arch/SIL/Hal/Clock
        ARK/Arch/SIL/Hal/Console
        ARK/Arch/SIL/Hal/Flash 
        ARK/Arch/SIL/Hal/Watchdog
        ARK/Arch/SIL/Hal/Pwm
        ARK/Arch/SIL/Hal/Adc
    )
endif()