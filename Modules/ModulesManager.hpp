#ifndef ARK_MODULES_MANAGER_HPP
#define ARK_MODULES_MANAGER_HPP

#include "ARK/Kernel/MicroKernels/ModulesKernel/ModulesKernel.hpp"
#include <cstdint>

namespace ARK {
    /**
     * @brief ModulesManager provides a high-level API for users to interact with
     *        hardware modules. It acts as a facade over ModulesKernel.
     */
    class ModulesManager {
    public:
        /**
         * @brief Register a custom module with the system
         * @param module Pointer to module implementing IModule interface
         * @return true if registration successful
         */
        static bool Register(IModule* module);
        /**
         * @brief Get a registered module by name
         * @param name Module identifier string
         * @return Pointer to module or nullptr if not found
         */
        static IModule* Get(const char* name);
        /**
         * @brief Check if all modules are operational
         * @return true if all modules report READY status
         */
        static bool AllReady();
        /**
         * @brief Get the total number of registered modules
         */
        static size_t Count();

        /**
         * @brief Get module status by name
         */
        static ModuleStatus Status(const char* name);
    };
}
#endif
