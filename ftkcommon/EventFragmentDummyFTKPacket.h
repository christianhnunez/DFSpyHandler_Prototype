#ifndef EVENTFRAGMENTDUMMYFTKPACKET_H_
#define EVENTFRAGMENTDUMMYFTKPACKET_H_

#include "ftkcommon/EventFragmentFTKPacket.h"
#include <iostream>

namespace daq {
  namespace ftk {

    /*! \brief Class representing a generic FTP event packet with the payload being data
     */
    class EventFragmentDummyFTKPacket : public EventFragmentFTKPacket
    {
    public:
      /*! \brief Connstructor, does nothing
       */
      EventFragmentDummyFTKPacket();
      /*! \brief Destructor, does nothing
       */
      virtual ~EventFragmentDummyFTKPacket();
      /* std::vector<unsigned int> m_data; */

    protected:
      /*! \brief Parses the payload of an FTK event packet
       *
       * Just copies the data into an internal vector
       *
       * \param data the raw payload
       */
      virtual void parsePayload(const std::vector<unsigned int>& data);

      /*! \brief Output the bitstream for the packet payload
       *
       * \return The raw data of the payload in the spy buffer
       */
      virtual std::vector<unsigned int> bitstreamPayload() const;


    private:
      std::vector<unsigned int> m_data;

    };

  } // namespace ftk
} // namespace daq

#endif // EVENTFRAGMENTDUMMYFTKPACKET_H_
