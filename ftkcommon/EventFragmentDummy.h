#ifndef EVENTFRAGMENTDUMMY_H_
#define EVENTFRAGMENTDUMMY_H_

#include "ftkcommon/EventFragment.h"
#include <iostream>

namespace daq {
  namespace ftk {

    /*! \brief Class representing a dummy object
     */
    class EventFragmentDummy : public EventFragment
    {
    public:
      /*! \brief Constructor, does nothing
       */
      EventFragmentDummy() { setL1ID(0xFFFFFFFF);};
      /*! \brief Destructor, does nothing
       */
      virtual ~EventFragmentDummy() {};

      /*! \brief Does nothing
       */
      virtual void parseFragment(const std::vector<unsigned int>& /*data*/) {};

      /*! \brief Does nothing
       */
      virtual std::vector<unsigned int> bitstream() const { return std::vector<unsigned int>(); };
    };

  } // namespace ftk
} // namespace daq

#endif // EVENTFRAGMENTDUMMY_H_
