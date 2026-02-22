#include "ModulesKernel.hpp"
#include "Modules/ModulesManager.hpp"


namespace ARK {

    void ModulesKernel::Initialize() {
        ARK::Modules::Initialize();
    }

    void ModulesKernel::Update() {
        ARK::Modules::Update();
    }

}