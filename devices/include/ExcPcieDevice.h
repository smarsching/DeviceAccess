#ifndef MTCA4U_EXCPCIEDEVICE_H
#define	MTCA4U_EXCPCIEDEVICE_H

#include "Exception.h"

namespace mtca4u{

class ExcPcieDevice : public ExcBase {
public:
    enum {EX_CANNOT_OPEN_DEVICE, EX_DEVICE_OPENED, EX_DEVICE_CLOSED, EX_READ_ERROR, EX_WRITE_ERROR,
	  EX_DMA_READ_ERROR, EX_DMA_WRITE_ERROR, EX_INFO_READ_ERROR, EX_UNSUPPORTED_DRIVER};
public:
    ExcPcieDevice(const std::string &_exMessage, unsigned int _exID);
private:

};

}//namespace mtca4u

#endif	/* MTCA4U_EXCPCIEDEVICE_H */

