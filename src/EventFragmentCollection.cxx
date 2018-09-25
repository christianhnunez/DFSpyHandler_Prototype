//#include "ftkcommon/EventFragmentCollection.h"
//#include "ftkcommon/EventFragmentDummy.h"
//#include "ftkcommon/core.h"
//#include "ftkcommon/exceptions.h"

//#include <algorithm>
//#include <iostream>

//namespace daq {
//  namespace ftk {

//    /***********************/
//    EventFragmentCollection::EventFragmentCollection(unsigned int L1ID)
//      : m_l1id(L1ID)
//	/***********************/
//    { }

//    /***********************/
//    EventFragmentCollection::~EventFragmentCollection()
//    /***********************/
//    {
//      for(unsigned int i=0; i < m_fragments.size(); i++)
//	delete m_fragments[i];
//      m_fragments.clear();
//    }

//    /***********************/
//    unsigned int
//    EventFragmentCollection::getL1ID() const
//    /***********************/
//    {
//      return m_l1id;
//    }

//    /***********************/
//    void
//    EventFragmentCollection::addEventFragment(EventFragment *fragment)
//    /***********************/
//    {
//      if(fragment->getL1ID() != m_l1id)
//	throw daq::ftk::EventFragmentCollectionAddWrongL1ID(ERS_HERE,m_l1id,fragment->getL1ID());
//      m_fragments.push_back(fragment);
//    }

//    /***********************/
//    EventFragment*
//    EventFragmentCollection::getEventFragment(unsigned int idx) const
//    /***********************/
//    {
//      if(m_fragments.size() > idx)
//	return m_fragments[idx];
//      else
//	return new EventFragmentDummy();
//    }

//    /***********************/
//    unsigned int
//    EventFragmentCollection::getNEventFragments() const
//    /***********************/
//    {
//      return m_fragments.size();
//    }

//    /***********************/
//    EventFragmentCollection*
//    EventFragmentCollection::subset(std::vector<unsigned int> fragmentIndexes)
//    /***********************/
//    {
//      EventFragmentCollection *subset = new EventFragmentCollection(m_l1id);

//      for(auto idx : fragmentIndexes)
//	{
//	  if(m_fragments.size() <= idx)
//	    throw daq::ftk::EventFragmentCollectionMissingIdx(ERS_HERE,idx,m_fragments.size());
//	  subset->addEventFragment(m_fragments[idx]);
//	}

//      return subset;
//    }

//    /***********************/
//    std::vector<EventFragmentCollection* >
//    build_eventFragmentCollection(std::vector<std::vector<EventFragment*> >& eventFragmentss,bool deletMemOutOfL1Range,unsigned int maskL1ID)
//    /***********************/
//    {
//      std::vector<EventFragmentCollection* > eventCollections;

//      // Determine range of the L1IDs
//      unsigned int first(0), last(0xffffffff); // mod L1A is 2**32

//      unsigned int first_temp, last_temp;
//      for (auto &eventFragments : eventFragmentss)
//	{
//	  if(eventFragments.size()==0) return eventCollections; // empty fragment set

//	  first_temp = eventFragments.front()->getL1ID()&maskL1ID;
//	  last_temp  = eventFragments.back() ->getL1ID()&maskL1ID;

//	  if(first_temp > first) first = first_temp;
//	  if(last_temp  < last)  last  = last_temp;
//	}

//      if (first > last)
//	{
//	  ERS_LOG("WARNING: no overlapping events!");
//	  return eventCollections;
//	}

//      if(deletMemOutOfL1Range)
//	{
//	  for (auto &eventFragments : eventFragmentss)
//	    {
//	      std::vector<EventFragment*>::iterator it;
//	      for(it=eventFragments.begin(); it!=eventFragments.end(); )
//		{
//		  if( (((*it)->getL1ID()&maskL1ID) < first) || (((*it)->getL1ID()&maskL1ID) > last) )
//		    {
//		      delete *it;
//		      it = eventFragments.erase(it);
//		      continue;
//		    }
//		  it++;
//		}
//	    }
//	}

//      // Find iterators for first and last L1IDs
//      std::vector<std::vector<EventFragment*>::const_iterator> firstIters;

//      //FIXME can be written more transparent?
//      auto is_first = [first,maskL1ID](EventFragment *fragment) {return (fragment->getL1ID()&maskL1ID)==first;};
//      for(auto &eventFragments : eventFragmentss)
//	{
//	  auto iter_first=std::find_if(std::begin(eventFragments),std::end(eventFragments),is_first);

//	  firstIters.push_back(iter_first);
//	}

//      // Start making the collections
//      unsigned int prev=first-1;
//      bool isL1IDGap=false;
//      for(unsigned int l1id=first;l1id<last+1;l1id++)
//	{
//	  unsigned int curr=(*(firstIters.at(0)))->getL1ID();

//	  if(curr!=l1id) continue;
//	  if(curr!=(prev+1)) isL1IDGap=true;

//	  EventFragmentCollection *collection=new EventFragmentCollection(l1id);

//	  for(auto& firstIter : firstIters)
//	    {
//	      collection->addEventFragment(*firstIter);
//	      firstIter++;
//	    }

//	  eventCollections.push_back(collection);
//	  prev=curr;
//	}
//      if(isL1IDGap) ERS_INFO("L1ID Gap is found in spy buffer contents");

//      return eventCollections;
//    }

//    std::vector<EventFragment* > stream_eventFragmentCollection(const std::vector<EventFragmentCollection*>& eventCollections, unsigned int idx)
//    {
//      std::vector<EventFragment* > result;
//      for(auto eventCollection : eventCollections)
//	{
//	  result.push_back(eventCollection->getEventFragment(idx));
//	}
//      return result;
//    }

//    std::vector<EventFragmentCollection* > subset_eventFragmentCollection(const std::vector<EventFragmentCollection*>& eventFragmentCollections, std::vector<unsigned int> fragmentIndexes)
//    {
//      std::vector<EventFragmentCollection* > subset;

//      for(auto eventFragmentCollection : eventFragmentCollections)
//	{
//	  EventFragmentCollection *subFragment=eventFragmentCollection->subset(fragmentIndexes);
//	  subset.push_back(subFragment);
//	}
//      return subset;
//    }
//  } // namespace ftk
//} // namespace daq
