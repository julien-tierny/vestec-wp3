/*! @file clustering.hpp
 * @brief provides a class for clustering results
 * @author Max Kontak <Max.Kontak@DLR.de>
 * @date 2019-05-10
 */

#ifndef CLUSTERING_HPP
#define CLUSTERING_HPP

// includes
#include <vector>

/** @brief class that stores clustering results
 **
 ** @tparam  Scalar  type used for scalar values
 **/
template< typename Scalar >
class Clustering
{
public:
  //! constructor of the Clustering class
  //!
  //! @param  distancesPerPointPerCluster  vector of vectors of distances of the points in a cluster to the corresponding centroid
  //!                                      distancesPerPointPerCluster[i][j] is the distance of the j'th point in the i'th cluster to that cluster's centroid
  Clustering( const std::vector< std::vector< Scalar > > &distancesPerPointPerCluster )
    : m_numberOfClusters            { distancesPerPointPerCluster.size() },
      m_numberOfPointsPerCluster    { [&distancesPerPointPerCluster]()
                                      {
                                        std::vector< int > output;
                                        output.reserve( distancesPerPointPerCluster.size() );

                                        for ( auto const &v : distancesPerPointPerCluster )
                                        {
                                          output.emplace_back( v.size() );
                                        }

                                        return output;
                                      }},
      m_distancesPerPointPerCluster { distancesPerPointPerCluster }
  {}

  //! return number of clusters in the clustering
  auto numberOfClusters() const
  {
    return m_numberOfClusters;
  }

  //! return vector of numbers of points per clusters
  auto numberOfPointsPerCluster() const
  {
    return m_numberOfPointsPerCluster;
  }

  //! return vector of vector of distances from the points of the cluster to the cluster's centroid
  //! return value's component [i][j] is the distance of the j'th point in the i'th cluster to that cluster's centroid
  auto distancesPerPointPerCluster() const
  {
    return m_distancesPerPointPerCluster;
  }



private:
  //! number of clusters in the clustering
  int                                  const m_numberOfClusters;

  //! vector of numbers of points per clusters
  std::vector< int >                   const m_numberOfPointsPerCluster;

  //! vector of vector of distances from the points of the cluster to the cluster's centroid
  std::vector< std::vector< Scalar > > const m_distancesPerPointPerCluster;



};

#endif // CLUSTERING_HPP
