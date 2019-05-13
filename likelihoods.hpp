/*! @file likelihoods.hpp
 * @brief provides free functions to compute likelihood functions for Clustering objects
 * @author Max Kontak <Max.Kontak@DLR.de>
 * @date 2019-05-10
 */

#ifndef LIKELIHOODS_HPP
#define LIKELIHOODS_HPP

// includes
#include "clustering_result.hpp"

#include <numeric>

namespace Likelihoods
{
//! compute the simplest likelihood for clustering, which is the negative residual sum of squares, normalized by the sample size
//!
//! @param  clustering  ClusteringResult class
//! @return             value of the log-likelihood function
template< typename Scalar >
auto residualSumOfSquares( ClusteringResult< Scalar > const & clustering )
{
  Scalar resSumOfSquares{ Scalar(0.0) };
  auto const distances{ clustering.distancesPerPointPerCluster() };

  auto const size{ std::accumulate(numPerCluster.begin(), numPerCluster.end(), Size(0) ) };

  for ( auto const & v : distances )
    for ( auto const & d : v )
      resSumOfSquares -= d*d/size;

  return resSumOfSquares;
}

}

#endif // LIKELIHOODS_HPP
