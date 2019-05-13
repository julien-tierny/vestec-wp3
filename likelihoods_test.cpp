/*! @file likelihoods_test.cpp
 * @brief test functionality of likelihoods functions
 * @author Max Kontak <Max.Kontak@DLR.de>
 * @date 2019-05-13
 */

#include "clustering_result.hpp"
#include "likelihoods.hpp"

#include <gtest/gtest.h>

#include <cmath>
#include <limits>

auto const tol{ std::sqrt(std::numeric_limits<double>::epsilon()) };

TEST( Likelihoods, residualSumOfSquares )
{
  {
    ClusteringResult<double> const clustering( { {1.0, 2.0, 3.0} } );

    auto const result{ Likelihoods::residualSumOfSquares( clustering ) };

    EXPECT_NEAR( result, -14./3. , tol );
  }
  {
    ClusteringResult<double> const clustering( { {0.0, 1.0, 2.0},
                                                 {3.0, 4.0},
                                                 {5.0},
                                                 {6.0, 7.0, 8.0, 9.0, 10.0} } );

    auto const result{ Likelihoods::residualSumOfSquares( clustering ) };

    EXPECT_NEAR( result, -385./11. , tol );
  }
}
