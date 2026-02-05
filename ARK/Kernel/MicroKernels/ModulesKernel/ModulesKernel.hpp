#ifndef ARK_MODULES_KERNEL_HPP
#define ARK_MODULES_KERNEL_HPP

#include <cstdint>
#include <cstddef>

namespace ARK {

    // Module status flags
    enum class ModuleStatus : uint8_t {
        DISABLED = 0,
        INITIALIZING,
        READY,
        ERROR,
        FAULT
    };

    // Base interface for all hardware modules
    class IModule {
    public:
        virtual ~IModule() = default;
        virtual bool Init() = 0;
        virtual void Update() = 0;
        virtual const char* GetName() const = 0;
        virtual ModuleStatus GetStatus() const = 0;
    };

    // Maximum num of modules can be registered
    constexpr size_t MAX_MODULES = 16;
    class ModulesKernel {
    public:
        static void Initialize();
        static void Update();

        // Module registration
        static bool RegisterModule(IModule* module);
        static void UnregisterModule(IModule* module);
        static IModule* GetModule(const char* name);
        static size_t GetModuleCount();
        static ModuleStatus GetModuleStatus(const char* name);
        // Communication protocol initialization
        static void InitializeCommunicationBuses();
        // Health monitoring
        static bool AreAllModulesReady();
        static uint32_t GetFaultyModulesMask();
    private:
        static IModule* modules[MAX_MODULES];
        static size_t moduleCount;
        static uint32_t moduleFaultMask;
        static void InitializeConfiguredModules();
        static void CheckModuleHealth();
    };
}
#endif
