
#include "ModulesManager.hpp"
namespace ARK {
    bool ModulesManager::Register(IModule* module) {
        return ModulesKernel::RegisterModule(module);
    }
    IModule* ModulesManager::Get(const char* name) {
        return ModulesKernel::GetModule(name);
    }
    bool ModulesManager::AllReady() {
        return ModulesKernel::AreAllModulesReady();
    }
    size_t ModulesManager::Count() {
        return ModulesKernel::GetModuleCount();
    }
    ModuleStatus ModulesManager::Status(const char* name) {
        return ModulesKernel::GetModuleStatus(name);
    }
}