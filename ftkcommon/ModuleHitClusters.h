#ifndef MODULEHITCLUSTERS_H_
#define MODULEHITCLUSTERS_H_

#include <iostream>
#include <vector>
#include <map>

#include "Cluster.h"


namespace daq {
  namespace ftk {


    class  ModuleHitClusters
    {
    public:

      ModuleHitClusters();

      ModuleHitClusters( ModuleHitClusters *other );

      virtual ~ModuleHitClusters();

      bool operator==( const ModuleHitClusters &other ) const;
//      friend std::ostream& operator<<(std::ostream& os, const ModuleHitClusters& mhc);

      void setModuleNumber(int v);
      void setModuleType(int v);
      int getModuleNumber() const;
      int getModuleType() const;

      /*! \brief Get copy of vector form of clusters
       */
      std::vector<PixelCluster*> getPixelVector() const;
      std::vector<SCTCluster*> getSCTVector() const;

      /*! \brief Get number of clusters in module
       */
      unsigned int getNClusters() const;
      SCTCluster* getSctCluster(unsigned int idx) const;
      PixelCluster* getPixelCluster(unsigned int idx) const;
      virtual void addCluster(const unsigned int & data);

    protected:
      virtual void parsePixelCluster(uint32_t word, PixelCluster * cluster);
      
    private:
      int m_module_number;
      int m_module_type;
      std::vector<PixelCluster*> m_pixel_clusters;
      std::vector<SCTCluster*> m_sct_clusters;

      // Hit cluster comparison functions:
      bool compareSCTClusters(std::vector<SCTCluster*> a, std::vector<SCTCluster*> b) const;
      bool comparePixelClusters(std::vector<PixelCluster*> a, std::vector<PixelCluster*> b) const;

    };


  } // namespace ftk
} // namespace daq
 
#endif //  MODULEHITCLUSTERS_H_
