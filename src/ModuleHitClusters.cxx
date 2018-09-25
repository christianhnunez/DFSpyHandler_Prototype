#include "/Users/ChristianHaroldNunez/Desktop/ftkcommon/ftkcommon/ModuleHitClusters.h"

#include "/Users/ChristianHaroldNunez/Desktop/ftkcommon/ftkcommon/Utils.h"
#include "/Users/ChristianHaroldNunez/Desktop/ftkcommon/ftkcommon/UtilsString.h"
#include "/Users/ChristianHaroldNunez/Desktop/ftkcommon/ftkcommon/core.h"


using namespace daq::ftk;

ModuleHitClusters::ModuleHitClusters()
  : m_module_number( -1 )
  , m_module_type( 0 )
{
// Delete clusters
for(auto cluster : m_sct_clusters)
  delete cluster;
m_sct_clusters.clear();
for(auto cluster : m_pixel_clusters)
  delete cluster;
m_pixel_clusters.clear();
}

ModuleHitClusters::ModuleHitClusters(ModuleHitClusters *other )
  : m_module_number( other->m_module_number )
  , m_module_type( other->m_module_type )
{
   m_pixel_clusters.clear();
   m_sct_clusters.clear();
  if( other->m_module_type ){
    for(unsigned int i = 0 ; i < other->m_sct_clusters.size() ; i++ ) m_sct_clusters.push_back(other->m_sct_clusters[i]);
   }
  else {
    for(unsigned int i = 0 ; i < other->m_pixel_clusters.size() ; i++ ) m_pixel_clusters.push_back(other->m_pixel_clusters[i]);
  }
}

ModuleHitClusters::~ModuleHitClusters()
{ }

bool ModuleHitClusters::operator==( const ModuleHitClusters &other ) const
{

  if(getModuleType() != other.getModuleType()) return false;
  if(getModuleNumber() != other.getModuleNumber()) return false;

  if(getModuleType() == 1) { // Compare SCT clusters
      return compareSCTClusters(getSCTVector(), other.getSCTVector());
  } else { // Compare pixel clusters
      return comparePixelClusters(getPixelVector(), other.getPixelVector());
  }
  return false; // Default. Happens if module type somehow is neither 0 nor 1.
}

//std::ostream& operator<<(std::ostream& os, const ModuleHitClusters& mhc)
//{
//    os << "ModuleHitCluster (ID " << mhc.getModuleNumber() << ", Type " << mhc.getModuleType() << "):";
//    return os;
//}


/*! \brief Get copy of vector form of clusters
 */
std::vector<PixelCluster*> ModuleHitClusters::getPixelVector() const{
    return m_pixel_clusters;
}
std::vector<SCTCluster*> ModuleHitClusters::getSCTVector() const {
    return m_sct_clusters;
}

void ModuleHitClusters::setModuleNumber(int v) {m_module_number = v;}
void ModuleHitClusters::setModuleType(int v) {m_module_type = v;}
int ModuleHitClusters::getModuleNumber() const {return m_module_number;}
int ModuleHitClusters::getModuleType() const {return m_module_type;}

/*! \brief Get number of clusters in module
 */
unsigned int ModuleHitClusters::getNClusters() const 
{
  if(m_module_type==1) return m_sct_clusters.size();
  else if(m_module_type==0) return m_pixel_clusters.size();
  else return 0;
}

SCTCluster* ModuleHitClusters::getSctCluster(unsigned int idx) const
{
  return m_sct_clusters[idx];
}

PixelCluster* ModuleHitClusters::getPixelCluster(unsigned int idx) const
{
  return m_pixel_clusters[idx];
}

void ModuleHitClusters::parsePixelCluster(uint32_t word, PixelCluster * cluster)
{
  cluster->column_width( getBitRange(word, 28, 30) );
  cluster->column_coord( getBitRange(word, 16, 27) );
  cluster->split_cluster(     getBit(word, 15)     );
  cluster->row_width(    getBitRange(word, 12, 14) );
  cluster->row_coord(    getBitRange(word,  0, 11) );
}

void ModuleHitClusters::addCluster(const unsigned int & cluster_word)
{
  if(m_module_type==1) 
    {
      // hit1
      daq::ftk::SCTCluster *cluster = new SCTCluster();
      cluster->hit_coord( getBitRange(cluster_word,  0, 10) );
      cluster->hit_width( getBitRange(cluster_word, 12, 14) );
      m_sct_clusters.push_back(cluster);
      // hit2
      if(!getBit(cluster_word, 15)){ // hit 2 present
	daq::ftk::SCTCluster *cluster = new SCTCluster();
	cluster->hit_coord( getBitRange(cluster_word, 16, 26) );
	cluster->hit_width( getBitRange(cluster_word, 28, 30) );
	m_sct_clusters.push_back(cluster);     
      }
      
    }
  else if(m_module_type==0) 
    {
      daq::ftk::PixelCluster *cluster = new PixelCluster;
      parsePixelCluster(cluster_word, cluster);
      m_pixel_clusters.push_back(cluster);
    }
  //else ERS_LOG("Unknown module type");
}

// O(n*logn) complexity
bool ModuleHitClusters::compareSCTClusters(std::vector<SCTCluster*> a, std::vector<SCTCluster*> b) const {
     if(a.size() != b.size()) return false;
     std::sort(a.begin(), a.end());
     std::sort(b.begin(), b.end());
     // Comparing elements:
     for(unsigned int i = 0; i < (unsigned int)(a.size()); i++) {
         if( !( *(a[i]) == *(b[i]) ) ) return false;
     }
     return true;
}

bool ModuleHitClusters::comparePixelClusters(std::vector<PixelCluster*> a, std::vector<PixelCluster*> b) const {
    if(a.size() != b.size()) return false;
    std::sort(a.begin(), a.end());
    std::sort(b.begin(), b.end());
    // Comparing elements:
    for(unsigned int i = 0; i < (unsigned int)(a.size()); i++) {
        if( !( *(a[i]) == *(b[i]) ) ) return false;
    }
    return true;

//    return std::equal(std::begin(a), std::end(a),
//                    std::begin(b),
//                    [](const PixelCluster* lhs, const PixelCluster* rhs){ return *lhs == *rhs; });
}




