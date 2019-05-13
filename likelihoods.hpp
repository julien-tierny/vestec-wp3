/*! @file likelihoods.hpp
 * @brief provides free functions to compute likelihood functions for Clustering objects
 * @author Max Kontak <Max.Kontak@DLR.de>
 * @date 2019-05-10
 */

#ifndef LIKELIHOODS_HPP
#define LIKELIHOODS_HPP

// includes
#include "clustering_result.hpp"

namespace Likelihoods
{
//! compute the simplest likelihood for clustering, which is the negative residual sum of squares
//!
//! @param  clustering  ClusteringResult class
//! @return             value of the log-likelihood function
template< typename Scalar >
auto residualSumOfSquares( ClusteringResult< Scalar > const & clustering )
{
  Scalar resSumOfSquares{ Scalar(0.0) };

  for ( auto const & v : clustering.distancesPerPointPerCluster() )
  {
    for ( auto const & d : v )
    {
      resSumOfSquares -= d*d;
    }
  }

  return resSumOfSquares;
}

}

#endif // LIKELIHOODS_HPP
