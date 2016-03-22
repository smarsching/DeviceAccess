/*
 * DeviceBackend.cc
 */

#include "RegisterAccessor.h"
#include "DeviceBackend.h"

namespace mtca4u {

  DeviceBackend::~DeviceBackend() {
  }

  void DeviceBackend::readDMA(uint8_t, uint32_t, int32_t*,  size_t) {
    std::cerr << "**********************************************************************************" << std::endl;
    std::cerr << " The function DeviceBackend::readDMA() was removed after deprecation." << std::endl;
    std::cerr << "**********************************************************************************" << std::endl;
    exit(1);
  }

  void DeviceBackend::writeDMA(uint8_t, uint32_t, int32_t const*,  size_t) {
    std::cerr << "**********************************************************************************" << std::endl;
    std::cerr << " The function DeviceBackend::readDMA() was removed after deprecation." << std::endl;
    std::cerr << "**********************************************************************************" << std::endl;
    exit(1);
  }

} /* namespace mtca4u */
