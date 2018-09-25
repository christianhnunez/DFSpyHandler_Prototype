//#include "ftkcommon/SpyBuffer.h"
//#include <iostream>
//#include <iomanip>

//int daq::ftk::SpyBuffer::checkFreezeBitsAndReadBuffer( const uint32_t& freeze_mask,
//                                                       bool ignore_freeze_bits )
//{
//  if ( (getSpyFreeze() & freeze_mask) || ignore_freeze_bits )
//    return readSpyBuffer();
//  else
//    return SPY_FAILED_FREEZE_CHECK;
//}

//int daq::ftk::SpyBuffer::unwrapBuffer()
//{
//  const uint32_t spyDim       = getSpyDimension();
//  const uint32_t lastAddress  = getSpyPointer()%spyDim;
//  // if the buffer wrapped, the chronologically oldest word is just after the last written one
//  const uint32_t firstAddress = (getSpyOverflow()) ? (lastAddress+1)%spyDim : 0;
//  // if the buffer didn't wrap, there are actually fewer words than the dimension
//  const uint32_t spySize      = (getSpyOverflow()) ? spyDim : (lastAddress+1)%spyDim;

//  std::vector<uint32_t> buffer_orig( std::move(m_buffer) );

//  if ( buffer_orig.size() < spyDim ) {
//    return -1;
//  }

//  m_buffer.clear();
//  m_buffer.reserve(spySize);

//  for ( uint32_t n=0; n < spySize; ++n ) {
//    uint32_t addr = (n+firstAddress)%spyDim;
//    m_buffer.push_back(buffer_orig[addr]);
//  }

//  return 0;
//}


//int daq::ftk::SpyBuffer::getVMEBoardNumber(int crate, int slot){

//  return (crate*21)+slot;

//}
