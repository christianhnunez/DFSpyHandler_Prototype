#ifndef EVENTFRAGMENTFTKPACKET_H_
#define EVENTFRAGMENTFTKPACKET_H_

#include "ftkcommon/EventFragment.h"
#include <iostream>

namespace daq {
  namespace ftk {

    /*! \brief Class representing a generic FTP event packet
     *
     * See the FTK_IM spec for the definition of header and trailer words.
     */
    class EventFragmentFTKPacket : public EventFragment
    {
    public:
      /*! \brief Connstructor, does nothing
       */
      EventFragmentFTKPacket();
      /*! \brief Destructor, does nothing
       */
      virtual ~EventFragmentFTKPacket();

      /*! \brief Parses a packet from data
       *
       * The entire contents of the FTK packet are expected to be inside data. This is
       * eveything between the 0xB0F0 and 0xE0F0 (inclusive) control words. Also the
       * 0xE0DA word must be doubled (ie: 0xE0DAE0DA).
       *
       * The payload is further parsed with the loadPayload function.
       *
       * \param data raw contents of the ftk event packet
       */
      virtual void parseFragment(const std::vector<unsigned int>& data);

      /*! \brief Output the bitstream for the packet
       *
       * Outputs the header and trailer. The content should be implemented by the 
       * subclasses in bitstreamPayload().
       *
       * \return The raw data of the spy buffer
       */
      virtual std::vector<unsigned int> bitstream() const;

      /*! \brief Get the LVL1ID in the trailer 
       */
      unsigned int getTrailerL1ID() { return m_trailer_lvl1id;};

    protected:
      /*! \brief Parses the payload of an ftk event packet
       *
       * Any class inheriting from EventFragmentFTKPacket must overload this function. This function
       * is responsible for parsing the payload (aka) data in the FTK event packet.
       *
       * The payload is defined as anything other than the header and trailer. The 0xE0DA is not
       * included in the payload.
       *
       * \param data the raw payload
       */
      virtual void parsePayload(const std::vector<unsigned int>& data) =0;

      /*! \brief Output the bitstream for the packet payload
       *
       * \return The raw data of the payload in the spy buffer
       */
      virtual std::vector<unsigned int> bitstreamPayload() const =0;


    private:
      unsigned int m_runNumber;
      unsigned int m_TTCrcBCID;
      unsigned int m_l1tt;
      unsigned int m_evTypROD;
      unsigned int m_TIM;
      unsigned int m_eFlags;
      unsigned int m_reservedTrailerWord;
      unsigned int m_lastTrailerWord;
      unsigned int m_trailer_lvl1id;

    };

  } // namespace ftk
} // namespace daq

/*! \brief Split a spybuffer containing FTK event packets into separate events
 *
 * An FTK event follows the format shown in the FTK_IM specification. Use the EventFragmentFTKPacket
 * for automatic parsing of the headers and trailers.
 *
 * The class used to parse the payload of an FTK packet is determined from the T template
 * argument.
 */
template<class T> std::vector<daq::ftk::EventFragment*> FTKPacket_splitFragments(const std::vector<unsigned int>& data, unsigned int ctrlPos)
{
  std::vector<daq::ftk::EventFragment*> events;
  
  bool processingEvent=false;
  unsigned int ctrlWord;
  std::vector<unsigned int> eventData;
  for(unsigned int i=0;i<data.size();i++)
    {
      ctrlWord=(data[i]>>ctrlPos)&0xFFFF;
      if(ctrlWord==0xB0F0)
	{
	  processingEvent=true;
	  eventData.clear();
	}

      if(!processingEvent) continue; //Not inside packet yet

      // Hack the E0DA position being different in different packet formats
      if(ctrlWord==0xE0DA)
	eventData.push_back(0xE0DAE0DA);
      else
	eventData.push_back(data[i]);

      // Check if this is the last word in a packet
      if(ctrlWord==0xE0F0)
	{
	  processingEvent=false;

	  daq::ftk::EventFragment *event=new T();
	  event->parseFragment(eventData);
	  events.push_back(event);
	}
    }

  return events;
}


#endif // EVENTFRAGMENTFTKPACKET_H_
