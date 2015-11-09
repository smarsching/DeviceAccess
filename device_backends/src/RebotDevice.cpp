#include "RebotDevice.h"
#include "exTcpCtrl.h"

using boost::asio::ip::tcp;

namespace mtca4u {

RebotDevice::RebotDevice(std::string boardAddr, int port)
:_boardAddr(boardAddr) , _port(port) , _tcpObject(boost::make_shared<TcpCtrl>(_boardAddr, _port))
{
}

RebotDevice::~RebotDevice() {
  if(isOpen()) {
    _tcpObject->closeConnection();
  }
}

void RebotDevice::open() {
  _tcpObject->openConnection();
  _opened = true;
}

void RebotDevice::read(uint8_t /*bar*/, uint32_t address, int32_t* data,  size_t sizeInBytes){
  if(!isOpen()) {
    throw exTcpCtrl("Device is closed", exTcpCtrl::EX_DEVICE_CLOSED);
  }
  if (sizeInBytes % 4 != 0) {
    throw exTcpCtrl("\"size\" argument must be a multiplicity of 4", exTcpCtrl::EX_SIZEMULT);
  }

  int mode = 3;
  boost::array<char, 4> receivedData;

  unsigned int datasendSize = 3*sizeof(int);
  std::vector<char> datasend(datasendSize);
  datasend[0] = mode;
  unsigned int packetsize = sizeInBytes/4;

  for (int j=1; j<4; ++j) {
    datasend[j] = 0;
  }
  for (int j=4; j<8; ++j) {
    datasend[j] = (address >> (8*(j-4))) & 0xFF;
  }
  for (int j=8; j<12; ++j) {
    datasend[j] = ((packetsize) >> (8*(j-8))) & 0xFF;
  }

  _tcpObject->sendData(datasend);
  _tcpObject->receiveData(receivedData);

  for(unsigned int i=0; i<packetsize; ++i) {
    int u32data = 0;
    _tcpObject->receiveData(receivedData);
    for(int j=0; j<4; ++j) {
      u32data |= ((receivedData[j] & 0xFF) << 8*j);
    }
    data[i] = u32data;
  }
}


void RebotDevice::write(uint8_t /*bar*/, uint32_t address, int32_t const* data,  size_t sizeInBytes){
  if (!isOpen()) {
    throw exTcpCtrl("Device is closed", exTcpCtrl::EX_DEVICE_CLOSED);
  }
  if (sizeInBytes % 4 != 0) {
    throw exTcpCtrl("\"size\" argument must be a multiplicity of 4", exTcpCtrl::EX_SIZEMULT);
  }

  int mode = 1;
  unsigned int packetsize = sizeInBytes/4;
  const unsigned int datasendSize = 3*sizeof(int);
  boost::array<char, 4> receivedData;
  for (unsigned int i=0; i<packetsize; ++i)  {
    std::vector<char> datasend(datasendSize);
    datasend[0] = mode;
    for (int j=1; j<4; ++j) {
      datasend[j] = 0;
    }
    for (int j=4; j<8; ++j) {
      datasend[j] = ((address+i) >> (8*(j-4))) & 0xFF;
    }
    for (int j=8; j<12; ++j) {
      datasend[j] = (data[i] >> (8*(j-8))) & 0xFF;
    }

    _tcpObject->sendData(datasend);
    _tcpObject->receiveData(receivedData);
  }
}

void RebotDevice::close() {
  _opened = false;
  _tcpObject->closeConnection();
}

} //namespace mtca4u
