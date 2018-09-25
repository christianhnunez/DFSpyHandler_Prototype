#ifndef EVENTFRAGMENTCOLLECTION_H_
#define EVENTFRAGMENTCOLLECTION_H_

#include "ftkcommon/EventFragment.h"

namespace daq {
  namespace ftk {

    /*! \brief Collection of event fragments in one event
     */
    class EventFragmentCollection
    {
    public:
      /*! \brief Create packet collection for event L1ID
       *
       * \param L1ID L1ID of event stored in this collection
       */
      EventFragmentCollection(unsigned int L1ID);

      /*! \brief Destructor, does nothing
       */
      ~EventFragmentCollection();

      /*! \brief Get L1ID
       *
       * \return L1ID of the event represented by this collection
       */
      unsigned int getL1ID() const;

      /*! \brief Add a new event fragment to collection
       *
       * \param fragment pointer to the fragment
       */
      void addEventFragment(EventFragment *fragment);

      /*! \brief Get event fragment
       *
       * \param idx index of fragment of interest
       */
      EventFragment* getEventFragment(unsigned int idx) const;

      /*! \brief Get number of fragments in collection
       *
       * \return number of fragments
       */  
      unsigned int getNEventFragments() const;

      /*! \brief Get subset of fragments
       *
       * \param fragmentIndexes list of event fragments to keep in subset
       *
       * \return new event fragment collection containing only specified fragments
       */  
      EventFragmentCollection* subset(std::vector<unsigned int> fragmentIndexes);

    private:
      unsigned int m_l1id;
      std::vector<EventFragment*> m_fragments;
    };

    /*! \brief Build an event fragment collection
     *
     * A list of event fragment lists is scanned for overlapping events
     * L1IDs in each list. For each overlapping event, a new event
     * fragment collection is created.
     *
     * \param eventfragments lists of event fragments to overlap
     * \param maskL1ID only use masked bits in L1ID comparisons
     *
     * \return list of valid event fragment collections
     */
    std::vector<EventFragmentCollection* > build_eventFragmentCollection(std::vector<std::vector<EventFragment*> >& eventfragmentss, bool deletMemOutOfL1Range=false, unsigned int maskL1ID=0xFFFFFFFF);

    /*! \brief Build a single spy buffer stream from an event fragment collection
     *
     * \param eventCollections List of event collections
     * \param idx The index of the event fragment to output
     *
     * \return List of event fragments at index idx inside the event collections 
     */
    std::vector<EventFragment* > stream_eventFragmentCollection(const std::vector<EventFragmentCollection*>& eventCollections, unsigned int idx);
    
    /*! \brief Build a new event fragment collection list containing only a subset of event fragments
     *
     * Run EventFragmentCollection::subset on each event fragment collection.
     *
     * \param eventfragment input event fragment collections
     * \param fragmentIndexes list of event fragments to keep in subset
     *
     * \return event fragment collections containing a subset of event fragments
     */
    std::vector<EventFragmentCollection* > subset_eventFragmentCollection(const std::vector<EventFragmentCollection*>& eventfragments, std::vector<unsigned int> fragmentIndexes);

   } // namespace ftk
} // namespace daq

#endif // EVENTFRAGMENTCOLLECTION_H_
