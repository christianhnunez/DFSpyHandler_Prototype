#include "ftkcommon/Cluster.h"

#include "ftkcommon/Utils.h"
#include "ftkcommon/UtilsString.h"
#include "ftkcommon/core.h"


using namespace daq::ftk;

SCTCluster::SCTCluster()
  : m_hit_coord(0),
    m_hit_width(0)
{
}

SCTCluster::SCTCluster(SCTCluster *other )
  : m_hit_coord(other->m_hit_coord),
    m_hit_width(other->m_hit_width)
{
}


SCTCluster::~SCTCluster()
{ }

bool SCTCluster::operator<( const SCTCluster &other ) const
{
    if(m_hit_coord < other.hit_coord()) return true;
    if(other.hit_coord() < m_hit_coord) return false;

    if(m_hit_width < other.hit_width()) return true;
    return false;
}

bool SCTCluster::operator==( const SCTCluster &other ) const
{
  return (m_hit_coord == other.hit_coord() && m_hit_width == other.hit_width());
}


PixelCluster::PixelCluster()
  :  m_column_width(0),
     m_column_coord(0),
     m_split_cluster(0),
     m_row_width(0),
     m_row_coord(0)
{
}

PixelCluster::PixelCluster(PixelCluster *other )
  :  m_column_width(other->m_column_width ),
     m_column_coord(other->m_column_coord ),
     m_split_cluster(other->m_split_cluster ),
     m_row_width(other->m_row_width ),
     m_row_coord(other-> m_row_coord )
{
}


PixelCluster::~PixelCluster()
{ }

bool PixelCluster::operator<( const PixelCluster &other) const
{
    if(m_column_width < other.column_width()) return true;
    if(other.column_width() < m_column_width) return false;

    if(m_column_coord < other.column_coord()) return true;
    if(other.column_coord() < m_column_coord) return false;

    if(m_split_cluster < other.split_cluster()) return true;
    if(other.row_coord() < m_split_cluster) return false;

    if(m_row_width < other.row_width()) return true;
    if(other.row_width() < m_row_width) return false;

    if(m_row_coord < other.row_coord()) return true;
    return false;
}

bool PixelCluster::operator==( const PixelCluster &other ) const
{
    return (m_column_width  == other.column_width()  &&
            m_column_coord  == other.column_coord()  &&
            m_split_cluster == other.split_cluster() &&
            m_row_width     == other.row_width()     &&
            m_row_coord     == other.row_coord());
}











