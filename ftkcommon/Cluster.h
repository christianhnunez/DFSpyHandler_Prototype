#ifndef CLUSTER_H_
#define CLUSTER_H_

#include <iostream>
#include <vector>
#include <map>


namespace daq {
  namespace ftk {

    class  PixelCluster
    {
    public:

      PixelCluster();
      
      PixelCluster( PixelCluster *other );
      
      ~PixelCluster();
      bool operator<( const PixelCluster &other ) const;
      bool operator==( const PixelCluster &other ) const;

      uint32_t column_width() const{return m_column_width;}
      uint32_t column_coord() const {return m_column_coord;}
      uint32_t split_cluster() const {return m_split_cluster;}
      uint32_t row_width() const {return m_row_width;}
      uint32_t row_coord() const {return m_row_coord;}

      void column_width(uint32_t value){m_column_width = value;}
      void column_coord(uint32_t value){m_column_coord = value;}
      void split_cluster(uint32_t value){m_split_cluster = value;}
      void row_width(uint32_t value){m_row_width = value;}
      void row_coord(uint32_t value){m_row_coord = value;}

    protected:
    private:
      uint32_t m_column_width;
      uint32_t m_column_coord;
      uint32_t m_split_cluster;
      uint32_t m_row_width;
      uint32_t m_row_coord;
    };
    
    
    class  SCTCluster
    {
    public:

      SCTCluster();
      
      SCTCluster( SCTCluster *other );
      
      ~SCTCluster();
      bool operator<( const SCTCluster &other ) const;
      bool operator==( const SCTCluster &other ) const;

      uint32_t hit_coord() const {return m_hit_coord;}
      uint32_t hit_width() const {return m_hit_width;}
      
      void hit_coord(uint32_t value) {m_hit_coord = value;}
      void hit_width(uint32_t value) {m_hit_width = value;}

    private:
      uint32_t m_hit_coord;
      uint32_t m_hit_width;
    };


  } // namespace ftk
} // namespace daq
 
#endif //  CLUSTER_H_
