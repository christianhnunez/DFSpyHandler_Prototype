#ifndef SPYBUFFER_H
#define SPYBUFFER_H

#include <vector>
#include <stdint.h>

namespace daq{
  namespace ftk{

    // SpyBuffer status word following common definition
    // See slides 23-24 of https://indico.cern.ch/event/679858/contributions/2857366/attachments/1585532/2507509/20180119_Unified_Error_Handling.pdf
    const uint32_t SPY_STATUS_LAST_WRITTEN_ADDRESS_MASK = 0x0000ffff;
    const uint32_t SPY_STATUS_FREEZE_MASK               = 0x00ff0000;
    const uint32_t SPY_STATUS_OVERFLOW_MASK             = 0x01000000;
    const uint32_t SPY_STATUS_SIZE_MASK                 = 0xf0000000;
    
    inline uint32_t get_spy_size_from_status( uint32_t status ) {
      uint32_t size_bits = ((status & SPY_STATUS_SIZE_MASK)>>28) & 0xf;
      return (0x1) << (size_bits+1); // 2^(size+1)
    }

    // Return value definitions
    const int SPY_FAILED_FREEZE_CHECK = 0x1;

    /**
     * Common base class to store spybuffer data
     * Derived classes are supposed to implement the virtual methods
     **/
    class SpyBuffer
    {
    public:
      
      /// Default constructor
      SpyBuffer() { m_spyStatus = 0; }
      /// Constructor setting m_spyStatus
      SpyBuffer(const uint32_t& spyStatus) { m_spyStatus = spyStatus; }

      /// Returns the payload (stored as std::vector<uint32_t>)
      std::vector<uint32_t> & getBuffer() { return m_buffer; }

      /// Getter and Setter for SpyStatus word
      uint32_t getSpyStatus() { return m_spyStatus; }
      void setSpyStatus(const uint32_t& spyStatus) { m_spyStatus = spyStatus; }

      virtual uint32_t getSpyFreeze()   ///< Returns freeze status
        { return ( m_spyStatus & SPY_STATUS_FREEZE_MASK ) >> 16; }
      virtual bool     getSpyOverflow() ///< Is the spyBuffer in overflow?         
        { return ( m_spyStatus & SPY_STATUS_OVERFLOW_MASK ); }
      virtual uint32_t getSpyPointer()  ///< Offset to the start of SpyBuffer data
        { return ( m_spyStatus & SPY_STATUS_LAST_WRITTEN_ADDRESS_MASK ); }
      virtual uint32_t getSpyDimension() ///< Dimension of spyBuffer data
        { return get_spy_size_from_status( m_spyStatus ); }

      // Access the spy status register from the board, setting m_spyStatus and spyStatus.
      virtual int readSpyStatusRegister( uint32_t& spyStatus ) = 0;

      // Check the spy status freeze bits against freeze_mask.
      // This assumes that m_spyStatus has already been set appropriately.
      // Read the buffer if ((freeze_mask & getSpyFreeze()) or ignore_freeze_bits)
      virtual int checkFreezeBitsAndReadBuffer( const uint32_t& freeze_mask=0x0,
                                                bool ignore_freeze_bits=false );

    protected:
      std::vector<uint32_t> m_buffer; ///< payload representation

      uint32_t m_spyStatus;

      // Clear m_buffer, then read the buffer from the board into it.
      // If necessary, derived classes should then call unwrapBuffer().
      virtual int readSpyBuffer() = 0;

      // Using the contents of m_spyStatus, infer the first and last written addresses.
      // Then, re-order m_buffer to represent the correct chronological order.
      virtual int unwrapBuffer();

      virtual int getVMEBoardNumber(int crate, int slot);

    };

  } //namespace ftk
}  //namespace daq

#endif
