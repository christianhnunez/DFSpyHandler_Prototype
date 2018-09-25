#ifndef EVENTFRAGMENT_H_
#define EVENTFRAGMENT_H_

#include <vector>
#include <string>

namespace daq {
  namespace ftk {

    /*! \brief Base class for event fragments
     * 
     * An event fragment is a class representing a single
     * event inside a spy buffer. It implements the tools
     * for extracting useful information.
     */
    class EventFragment
    {
    public:
      /*! \brief Constructor, does nothing
       */
      EventFragment();
      /*! \brief Destructor, does nothing
       */
      virtual ~EventFragment();

      /*! \brief Returns L1ID of the event
       *
       * \param L1ID of the event
       */
      unsigned int getL1ID() const;

      /*! \brief Set the L1ID for the event
       *
       * \param l1id The new L1ID
       */      
      void setL1ID(unsigned int l1id);

      /*! \brief Code for loading data from a spybuffer
       *
       * \param data The raw data corresponding to the event
       */
      virtual void parseFragment(const std::vector<unsigned int>& data) =0;

      /*! \brief Generate the raw data for the spy buffer
       *
       * \return The raw data of the spy buffer
       */
      virtual std::vector<unsigned int> bitstream() const =0;

    private:
      unsigned int m_l1id;
    };

    /*! \brief Dump bitstream for an event colleciton.
     *
     * \param eventFragments List of fragments to dump.
     * \param outPath Path to the output file
     */
    void dump_eventFragments(const std::vector<EventFragment*>& eventFragments, const std::string& outPath);
  } // namespace ftk
} // namespace daq

#endif // EVENTFRAGMENT_H_
