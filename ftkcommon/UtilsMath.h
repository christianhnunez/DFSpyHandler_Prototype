#ifndef UTILSMATH_H_
#define UTILSMATH_H_

#include <assert.h>
#include <algorithm>
#include <limits>
#include <cmath>

/*
 *
 * Some useful mathematical functions based on TMath.h from ROOT
 *
 */

// Define integer used in TF math
typedef int64_t Int_tf;

namespace daq
{
  namespace ftk
  {
    template<typename T>
      inline int Nint( T x )
      {
	int i;
	if (x >= 0)
	  {
	    i = int(x + 0.5);
	    if ( i & 1 && x + 0.5 == T(i) ) i--;
	  }
	else
	  {
	    i = int(x - 0.5);
	    if ( i & 1 && x - 0.5 == T(i) ) i++;
	  }
	return i;
      }

    inline int FloorNint(double x)
    { return Nint( floor(x) ); }

    inline unsigned int Powerof2(unsigned int n)
    {
      return (1<<n);
    }
  }
}
#endif // UTILSMATH_H_
