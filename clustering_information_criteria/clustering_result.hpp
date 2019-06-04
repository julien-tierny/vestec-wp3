/*! @file clustering_result.hpp
 * @brief provides a class for clustering results
 * @author Max Kontak <Max.Kontak@DLR.de>
 * @date 2019-05-10
 */

#ifndef CLUSTERING_RESULT_HPP
#define CLUSTERING_RESULT_HPP

// includes
#include <fstream>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>

/** @brief class that stores clustering results
 **
 ** @tparam  Scalar  type used for scalar values
 **/
template< typename Scalar >
class ClusteringResult
{
public:
  //! constructor of the Clustering class
  //!
  //! @param  distancesPerPointPerCluster  vector of vectors of distances of the points in a cluster to the corresponding centroid
  //!                                      distancesPerPointPerCluster[i][j] is the distance of the j'th point in the i'th cluster to that cluster's centroid
  ClusteringResult( std::vector< std::vector< Scalar > > const & distancesPerPointPerCluster )
    : m_numberOfClusters            { distancesPerPointPerCluster.size() },
      m_numberOfPointsPerCluster    { [&distancesPerPointPerCluster]()
                                      {
                                        std::vector< typename std::vector<Scalar>::size_type > output;
                                        output.reserve( distancesPerPointPerCluster.size() );

                                        for ( auto const &v : distancesPerPointPerCluster )
                                        {
                                          output.emplace_back( v.size() );
                                        }

                                        return output;
                                      }() },
      m_distancesPerPointPerCluster { distancesPerPointPerCluster },
      m_numberOfPoints{ [&distancesPerPointPerCluster]()
                        {
                          typename std::vector<std::vector<Scalar>>::size_type output( 0 );

                          for (auto const & v : distancesPerPointPerCluster )
                            output += v.size();

                          return output;
                        }() }
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

  //! return total number of points
  auto numberOfPoints() const
  {
    return m_numberOfPoints;
  }


private:
  //! number of clusters in the clustering
  typename std::vector<std::vector<Scalar>>::size_type   const m_numberOfClusters            {};

  //! vector of numbers of points per clusters
  std::vector< typename std::vector<Scalar>::size_type > const m_numberOfPointsPerCluster    {};

  //! vector of vector of distances from the points of the cluster to the cluster's centroid
  std::vector< std::vector< Scalar > >                   const m_distancesPerPointPerCluster {};

  //! total number of samples
  typename std::vector<std::vector<Scalar>>::size_type   const m_numberOfPoints              {};
};

template< typename Scalar, typename Stream >
decltype(auto) clusteringResultFromASCIIStream( Stream & input )
{
  std::vector< std::vector< Scalar > > distancesPerPointPerCluster;

  std::string line{};

  while( std::getline( input, line ) )
  {
    std::istringstream is{ line };

    std::vector<Scalar> row{};

    std::copy( std::istream_iterator<Scalar>( is ),
               std::istream_iterator<Scalar>(),
               std::back_inserter( row ) );

    distancesPerPointPerCluster.push_back( row );
  }

  return ClusteringResult<Scalar>{ distancesPerPointPerCluster };
}

#endif // CLUSTERING_RESULT_HPP
