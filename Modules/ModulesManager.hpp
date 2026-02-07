#ifndef ARK_MODULES_MANAGER_HPP
#define ARK_MODULES_MANAGER_HPP

#include "BMP280/BMP280.hpp"
#include "MPU9250/MPU9250.hpp"
#include "L89/L89.hpp"
#include "QMC5883L/QMC5883L.hpp"
#include "NRF24/NRF24.hpp"

namespace ARK {
namespace Modules {
    void Initialize();
    void Update(); 
}
}

#endif
