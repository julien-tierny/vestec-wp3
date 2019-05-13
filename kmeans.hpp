/*! @file kmeans.hpp
 * @brief provides a simple implementation of k-means clustering for 2d data
 * @author Max Kontak <Max.Kontak@DLR.de>
 * @date 2019-05-13
 */

#ifndef KMEANS_HPP
#define KMEANS_HPP

// includes
#include "clustering_result.hpp"

#include <array>
#include <iostream>
#include <stdexcept>
#include <vector>

namespace KMeans
{
  //! type used for 2d vectors
  template<typename Scalar>
  using vec = std::array<Scalar, 2>;

  //! squared distance between two vec's
  template<typename Scalar>
  auto squaredDistance( vec<Scalar> u, vec<Scalar> v)
  {
    return (u[0]-v[0])*(u[0]-v[0])
           + (u[1]-v[1])*(u[1]-v[1])
           + (u[2]-v[2])*(u[2]-v[2]);
  }

  //! sum operator for vec
  template<typename Scalar>
  vec<Scalar> operator+( vec<Scalar> const & u, vec<Scalar> const & v )
  {
    return { u[0]+v[0], u[1]+v[1] };
  }

  //! in-place sum operator for vec
  template<typename Scalar>
  void operator+=( vec<Scalar> & u, vec<Scalar> const & v)
  {
    u[0] += v[0];
    u[1] += v[1];
  }

  //! multiplication by scalar for vec
  template<typename Scalar>
  vec<Scalar> operator*( Scalar const & a, vec<Scalar> const & u )
  {
    return { a*u[0], a*u[1] };
  }

  //! @brief Lloyd's algorithm for k-means clustering
  //!
  //! @param  data           2d data points
  //! @param  numClusters    desired number of clusters ("k")
  //! @param  maxIterations  maximum number of iterations
  //! @return                ClusteringResult class
  template< typename Scalar >
  auto lloyd_algorithm(          std::vector< vec<Scalar> >       const & data,
                        typename std::vector< Scalar >::size_type         numClusters,
                                 int                                      maxIterations = 100 )
  {
    if ( numClusters > data.size() )
      throw std::length_error("There must be less clusters than data points.");

    // initialization (with first data elements)
    std::vector< vec<Scalar> > centroids;

    std::copy( data.begin(), data.begin()+numClusters, std::back_inserter(centroids) );

    // vector with cluster number for every data point
    std::vector< decltype(numClusters) > belongsToCluster( data.size() );

    // flag to capture if clustering has changed
    bool hasChanged{ true };

    // iteration
    for (int i=0; i<maxIterations; ++i)
    {
      hasChanged = false;

      // searching nearest centroid for every data point
      for (decltype(data.size()) j=0; j<data.size(); ++j )
      {
        Scalar                minDistance { squaredDistance(data[j], centroids[0]) };
        decltype(numClusters) minIndex    { 0U };

        for (decltype(numClusters) k=1; k<numClusters; ++k)
        {
          Scalar const distance{ squaredDistance( data[j], centroids[k] ) };

          if (distance < minDistance)
          {
            minDistance = distance;
            minIndex    = k;
          }
        }

        if (minIndex != belongsToCluster[j])
          hasChanged = true;

        belongsToCluster[j] = minIndex;
      }

      // compute new centroids
      std::vector< vec<Scalar> >           sumOfPoints      ( numClusters );
      std::vector< decltype(data.size()) > numberPerCluster ( numClusters );

      for (decltype(data.size()) j=0; j<data.size(); ++j )
      {
        sumOfPoints[belongsToCluster[j]] += data[j];
        numberPerCluster[belongsToCluster[j]]++;
      }

      std::cout << "k=" << numClusters << ", i=" << i << ", centroids: ";

      for (decltype(numClusters) k=0; k<numClusters; ++k )
      {
        centroids[k] = (1.0/numberPerCluster[k])*( sumOfPoints[k] );

        std::cout << "{ " << centroids[k][0] << ", " << centroids[k][1] << " }, ";
      }

      std::cout << "\b\b  " << std::endl;

      if (!hasChanged)
        break;
    }

    // provide ClusteringResult
    std::vector< std::vector< Scalar > > distancesPerCluster( numClusters );

    for (decltype(data.size()) i=0; i<data.size(); ++i)
    {
      distancesPerCluster[belongsToCluster[i]].push_back( squaredDistance( data[i], centroids[belongsToCluster[i]] ) );
    }

    return ClusteringResult< Scalar >{distancesPerCluster};
  }
}

#endif // KMEANS_HPP
