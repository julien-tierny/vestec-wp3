/*! @file likelihoods.hpp
 * @brief provides free functions to compute likelihood functions for Clustering objects
 * @author Max Kontak <Max.Kontak@DLR.de>
 * @date 2019-05-10
 */

#ifndef LIKELIHOODS_HPP
#define LIKELIHOODS_HPP

// includes
#include "clustering_result.hpp"

#include <cmath>
#include <numeric>

namespace Likelihoods
{

//! pi
double const pi{ 4*std::atan(1.) };

namespace impl
{

template< typename Scalar >
auto residualSumOfSquares( ClusteringResult<Scalar> const & clustering )
{
  Scalar       RSS       { Scalar(0.0) };

  auto   const distances { clustering.distancesPerPointPerCluster() };

  for ( auto const & v : distances )
    for ( auto const & d : v )
      RSS += d * d;

  return RSS;
}

}

//! compute the simplest likelihood for clustering, which is the negative residual sum of squares, normalized by the sample size
//!
//! This likelihood is based on a Gaussian mixture model, where it is assumed that all clusters are of the same size and have the same standard deviation
//!
//! @param  clustering  ClusteringResult class
//! @return             value of the log-likelihood function
template< typename Scalar >
auto unitVariance( ClusteringResult< Scalar > const & clustering )
{
  return -impl::residualSumOfSquares( clustering )/(2.0 * clustering.numberOfPoints());
}

template< typename Scalar, typename Integer >
auto sameVariance( ClusteringResult< Scalar > const & clustering,
                   Integer                    const & dim )
{
  Scalar logLikelihood = 0.0;

  auto const N { clustering.numberOfPoints() };
  auto const K { clustering.numberOfClusters() };

  // compute variance
  auto const var { 1.0/(dim*(N-K)) * impl::residualSumOfSquares(clustering) };

  // compute log-likelihood
  for ( size_t k=0; k<clustering.numberOfClusters(); ++k )
    logLikelihood += clustering.numberOfPointsPerCluster()[k] * std::log(clustering.numberOfPointsPerCluster()[k]);

  logLikelihood -= N * std::log(N);

  logLikelihood -= 0.5 * N * dim * std::log(2 * pi * (var*var));

  logLikelihood -= 0.5 * dim * ( N - K );

  return logLikelihood;
}

}

#endif // LIKELIHOODS_HPP
