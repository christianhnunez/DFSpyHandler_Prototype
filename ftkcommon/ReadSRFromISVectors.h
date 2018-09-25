#ifndef __READSRFROMISVECTOR_H__
#define __READSRFROMISVECTOR_H__

#include <vector>  		// std::vector	

namespace daq {
	namespace ftk {
		/*! \brief Read status register content by SR address using list of SR collections and corresponding meta info obtained from IS. Return 0 if address is not stored.
		 * \param srAdress Status register address.
		 * \param srList Vector of IS status register vectors.
		 * \param srInfoList Corresponding meta info containig [first address, last address, address increment] of IS status register vectors.
		 * \param selectorRegister Selector register address.
		 */
		unsigned int ReadSRFromISVectors( unsigned int srAdress, const std::vector< std::vector<unsigned int> > &srList, const std::vector< std::vector<unsigned int> > &srInfoList, unsigned int fpgaNum = 0, unsigned int selectorRegister = 0, unsigned int readerRegister = 0 );



	} // namespace ftk
} // namespace daq



#endif // __READSRFROMISVECTOR_H__
