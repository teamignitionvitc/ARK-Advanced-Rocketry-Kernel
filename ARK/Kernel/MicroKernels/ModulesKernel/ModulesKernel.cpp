
#include "ModulesKernel.hpp"
#include "ARK/HAL/Console/Console.hpp"
#include "ARK/System/CommunicationProtocols/I2c/I2c.hpp"
#include "ARK/System/CommunicationProtocols/Spi/Spi.hpp"
#include "ARK/System/CommunicationProtocols/Gpio/Gpio.hpp"
#include "ARK/System/FDIR/FDIR.hpp"
#include "ARK/CoreTypes.hpp"
#include "Config.hpp"
#include <cstring>
namespace ARK {
    //definitions
    IModule* ModulesKernel::modules[MAX_MODULES] = {nullptr};
    size_t ModulesKernel::moduleCount = 0;
    uint32_t ModulesKernel::moduleFaultMask = 0;
    void ModulesKernel::Initialize() {
        SystemConsole.Print("[MODULES] ModulesKernel Initializing");
        // Initialize buses
        InitializeCommunicationBuses();
        // Initialize any pre-configured modules, used Config.hpp here
        InitializeConfiguredModules();
        // Initialize for all the modules registered
        for (size_t i = 0; i < moduleCount; ++i) {
            if (modules[i] != nullptr) {
                SystemConsole.Print("Initializing module: %s", modules[i]->GetName());
                if (!modules[i]->Init()) {
                    SystemConsole.Error("Failed to initialize module: " + std::string(modules[i]->GetName()));
                    moduleFaultMask |= (1 << i);
                } else {
                    SystemConsole.Success("Module initialized: " + std::string(modules[i]->GetName()));
                }
            }
        }
        //Give module count post Initialization
        SystemConsole.Print("ModulesKernel Initialized. %zu modules registered.", moduleCount);
    }

    void ModulesKernel::Update() {
        // Update registered modules
        for (size_t i = 0; i < moduleCount; ++i) {
            if (modules[i] != nullptr) {
                ModuleStatus status = modules[i]->GetStatus();
                if (status == ModuleStatus::READY) {
                    modules[i]->Update();
                } else if (status == ModuleStatus::ERROR || status == ModuleStatus::FAULT) {
                    // Track faulty modules
                    moduleFaultMask |= (1 << i);
                }
            }
        }
        // Periodic health check, as mentioned to use FDIR
        CheckModuleHealth();
    }

    void ModulesKernel::InitializeCommunicationBuses() {
        SystemConsole.Print("Initializing communication buses...");
        // I2C Bus initialization (commonly used for sensors)  
        #if IS_IMU || IS_ALTIMETER || IS_GYROSCOPE
        I2C::Setup(400000); // Change the frequency as needed (e.g. 400kHz here)
        SystemConsole.Print("I2C bus initialized at 400kHz");
        #endif
        // SPI Bus initialization (commonly used for high-speed peripherals)
        #if IS_STORAGE || IS_COMMUNICATION
        SPI::Setup(1000000); // Change the frequency as needed (e.g. 1MHz here)
        SystemConsole.Print("SPI bus initialized at 1MHz");
        #endif

        SystemConsole.Print("Communication buses ready.");
    }

    void ModulesKernel::InitializeConfiguredModules() {
        SystemConsole.Print("Checking configured modules...");  
        // Based on Config.hpp flags, initialize relevant pins

        // Actuators (Servos/Motors)
        #if IS_ACTUATORS
        SystemConsole.Print("Actuators module enabled");
        // Initialize servo/motor control pins
        #endif
        //barometer/altimeter
        #if IS_ALTIMETER
        SystemConsole.Print("Altimeter module enabled");
        #endif
        //IMU/MPU6050 etc.
        #if IS_IMU
        SystemConsole.Print("IMU module enabled");
        #endif
        //Gyroscope/MPU6500 etc.
        #if IS_GYROSCOPE
        SystemConsole.Print("Gyroscope module enabled");
        #endif
        //Positioning (GPS)
        #if IS_POSITIONING
        SystemConsole.Print("Positioning (GPS) module enabled");
        #endif
        // Communication/NRF modules
        #if IS_COMMUNICATION
        SystemConsole.Print("Communication module enabled");
        #endif
        // Storage (SD card, Flash)
        #if IS_STORAGE
        SystemConsole.Print("Storage module enabled");
        #endif
        // Recovery/Ejection (Mosfets channels)
        #if IS_RECOVERY_EJECTION
        SystemConsole.Print("Recovery/Ejection module enabled");
        // Initialize pyro channels GPIO
        #endif
        // Analog Inputs if necessary
        #if IS_ANALOG
        SystemConsole.Print("Analog input module enabled");
        #endif
        //Audio, speaker etc.
        #if IS_AUDIO
        SystemConsole.Print("Audio module enabled");
        #endif
        //Payload if Present
        #if IS_PAYLOAD
        SystemConsole.Print("Payload module enabled");  
        #endif
        //LED or Indicators
        #if IS_LIGHT
        SystemConsole.Print("Light (LED) module enabled");
        #endif
    }

    bool ModulesKernel::RegisterModule(IModule* module) {
        if (module == nullptr) {
            SystemConsole.Error("Cannot register null module");
            return false;
        }

        if (moduleCount >= MAX_MODULES) {
            SystemConsole.Error("Module registry full, cannot register: " + std::string(module->GetName()));
            return false;
        }

        // Check if by chance any duplicate registration
        for (size_t i = 0; i < moduleCount; ++i) {
            if (modules[i] == module) {
                SystemConsole.Error("Module already registered: " + std::string(module->GetName()));
                return false;
            }
        }

        modules[moduleCount++] = module;
        SystemConsole.Print("Module registered: %s (slot %zu)", module->GetName(), moduleCount - 1);
        return true;
    }

    void ModulesKernel::UnregisterModule(IModule* module) {
        if (module == nullptr) return;

        for (size_t i = 0; i < moduleCount; ++i) {
            if (modules[i] == module) {
                SystemConsole.Print("Unregistering module: %s", module->GetName());
                // Shift remaining modules
                for (size_t j = i; j < moduleCount - 1; ++j) {
                    modules[j] = modules[j + 1];
                }
                modules[--moduleCount] = nullptr;
                // Clear fault bit and shift fault mask
                moduleFaultMask &= ~(1 << i);
                return;
            }
        }
    }

    IModule* ModulesKernel::GetModule(const char* name) {
        if (name == nullptr) return nullptr;
        for (size_t i = 0; i < moduleCount; ++i) {
            if (modules[i] != nullptr && std::strcmp(modules[i]->GetName(), name) == 0) {
                return modules[i];
            }
        }
        return nullptr;
    }

    size_t ModulesKernel::GetModuleCount() {
        return moduleCount;
    }

    ModuleStatus ModulesKernel::GetModuleStatus(const char* name) {
        IModule* module = GetModule(name);
        if (module != nullptr) {
            return module->GetStatus();
        }
        return ModuleStatus::DISABLED;
    }

    bool ModulesKernel::AreAllModulesReady() {
        if (moduleCount == 0) return true;

        for (size_t i = 0; i < moduleCount; ++i) {
            if (modules[i] != nullptr) {
                ModuleStatus status = modules[i]->GetStatus();
                if (status != ModuleStatus::READY) {
                    return false;
                }
            }
        }
        return true;
    }

    uint32_t ModulesKernel::GetFaultyModulesMask() {
        return moduleFaultMask;
    }

    void ModulesKernel::CheckModuleHealth() {
        for (size_t i = 0; i < moduleCount; ++i) {
            if (modules[i] != nullptr) {
                ModuleStatus status = modules[i]->GetStatus();
                if (status == ModuleStatus::FAULT) {
                    moduleFaultMask |= (1 << i);
                    // Raise appropriate FDIR(Fault detection, Isolation and Recovery)
                    // Action to be taken by System Kernel.
                }
            }
        }

        // If any critical fault, raise system fault
        if (moduleFaultMask != 0) {
            //can add specific faults later on
        }
    }

}