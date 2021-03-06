#include "DummyBackend.h"
#include "BackendFactory.h"
#include "DeviceAccessVersion.h"

using namespace mtca4u;
#define WRONG_VERSION "00.18"

// LCOV_EXCL_START these lines cannot be reached because the backend cannot be registered, thus it is never instantiated
struct WrongVersionBackend : public DummyBackend{
  using DummyBackend::DummyBackend;
  static boost::shared_ptr<DeviceBackend> createInstance(std::string /*host*/, std::string instance, std::list<std::string> parameters, std::string /*mapFileName*/){
    return returnInstance<WrongVersionBackend>(instance, convertPathRelativeToDmapToAbs(parameters.front()));
  }

// LCOV_EXCL_STOP The registerern and the version functions have to be called

  struct BackendRegisterer{
    BackendRegisterer(){
      mtca4u::BackendFactory::getInstance().registerBackendType("wrongVersionBackend","",&WrongVersionBackend::createInstance, WRONG_VERSION);
    }
  };

};

static WrongVersionBackend::BackendRegisterer gWrongVersionBackendRegisterer;


extern "C"{
  const char * deviceAccessVersionUsedToCompile(){
    return WRONG_VERSION;
  }
}
