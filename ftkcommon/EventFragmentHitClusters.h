#ifndef EVENTFRAGMENTHITCLUSTERS_H_
#define EVENTFRAGMENTHITCLUSTERS_H_

#include "ftkcommon/EventFragmentFTKPacket.h"
#include "ftkcommon/ModuleHitClusters.h"
#include "ftkcommon/core.h"

namespace daq {
  namespace ftk {


    /*! \brief Class representing IM Hit Cluster 
     *
     * See the FTK_IM specification for the definition of header and trailer words.
     */
    class EventFragmentHitClusters : public EventFragmentFTKPacket
    {
    public:
      EventFragmentHitClusters();

      virtual ~EventFragmentHitClusters();

      /*! \brief Get number of modules in payload
       */
      unsigned int getNModules() const;
      
      /*! \brief Get module at index idx
       */
      std::shared_ptr<ModuleHitClusters> getModule(unsigned int idx) const;

      protected:
      /*! \brief nothing
       */
      virtual void parsePayload(const std::vector<unsigned int>& data);
      
      /*! \brief Output the bitstream for the packet payload
       *
       * \return The raw data of the payload in the spy buffer
       */
      virtual std::vector<unsigned int> bitstreamPayload() const;
      
    private:
      std::vector<unsigned int> m_data;
      std::vector<std::shared_ptr<ModuleHitClusters> > m_modules;
    
    };

  } // namespace ftk
} // namespace daq

/*! \brief Split a spybuffer containing FTK event packets into separate events and parse the payload. Handles splitted spy buffers.
 *
 * An FTK event follows the format shown in the FTK_IM specification. Use the EventFragmentFTKPacket
 * for automatic parsing of the headers and trailers.
 *
 * The class used to parse the payload of an FTK packet is determined from the T template
 * argument.
 */
//template<class T> std::vector<T*> FTKPacket_parseFragments(const std::vector<unsigned int>& data, unsigned int ctrlPos)
template<class T> std::vector<std::shared_ptr<T> > FTKPacket_parseFragments(const std::vector<unsigned int>& data, unsigned int ctrlPos)
{
  //std::vector<T*> events;
  std::vector<std::shared_ptr<T> > events;
  bool processingEvent=false;
  bool is_splittedFragment = false;
  unsigned int ctrlWord;
  std::vector<unsigned int> eventData;
  do
  {
    for(unsigned int i=0;i<data.size();i++)
    {
      // Check for ctrl words
      ctrlWord=(data[i]>>ctrlPos)&0xFFFF;
      if(ctrlWord==0xB0F0)
        {
          // take care of fragment which is not contiguous
          // splitted fragment is not completely contained in spy buffer
          if(is_splittedFragment) {
        is_splittedFragment = false;
        break;
          }
//	      ERS_DEBUG(1,"Start processing event fragment");
//        std::cout << "Start processing event fragment" << std::endl;
          processingEvent=true;
          eventData.clear();
        }

      if(!processingEvent) continue; //Not inside packet yet
	  
      // Hack the E0DA position being different in different packet formats
      if(ctrlWord==0xE0DA) {
          eventData.push_back(0xE0DAE0DA);
      } else {
          eventData.push_back(data[i]);
      }
      // Check if this is the last word in a packet
      if(ctrlWord==0xE0F0)
        {
          processingEvent=false;
//	      ERS_DEBUG(1,"Finished processing event fragment");
//        std::cout << "Finished processing event fragment" << std::endl;

          //T *event=new T();
          std::shared_ptr<T> event = std::make_shared<T>();


//          ERS_DEBUG(1,"Start parsing event fragment");
//        std::cout << "Start parsing event fragment" << std::endl;
          event->parseFragment(eventData);
//	      ERS_DEBUG(1,"Finished parsing event fragment");
//        std::cout << "Finished parsing event fragment" << std::endl;

          if(event->getL1ID()==event->getTrailerL1ID()) {
              events.push_back(event);
          }
          if(is_splittedFragment) break; // take care of fragment which is not contiguous
          // take care of fragment which is not contiguous
          // splitted fragment is completely contained in spy buffer
          if(is_splittedFragment) {
        is_splittedFragment = false;
        break;
          }
	      
        }
	  
      // start again from beginning if event was not completely processed
      if(i==(data.size()-1) && processingEvent) {
        is_splittedFragment = true;
//	    ERS_DEBUG(1,"Split fragment detected");
        std::cout << "Split fragment detected" << std::endl;
      }
    }
    }while(is_splittedFragment);

  return events;
}


#endif // EVENTFRAGMENTHITCLUSTERS_H_
