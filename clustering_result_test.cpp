/*! @file clustering_test.cpp
 * @brief test functionality of clustering class
 * @author Max Kontak <Max.Kontak@DLR.de>
 * @date 2019-05-10
 */

#include "clustering_result.hpp"

#include <gtest/gtest.h>

#include <cmath>
#include <limits>

auto const tol{ std::sqrt(std::numeric_limits<double>::epsilon()) };

TEST( ClusteringResult, Initialization )
{
  // provide input for ClusteringResult constructor
  std::vector< std::vector< double > > const distancesPerPointPerCluster = { {0.0, 1.0, 2.0},
                                                                             {3.0, 4.0},
                                                                             {5.0},
                                                                             {6.0, 7.0, 8.0, 9.0, 10.0} };

  ClusteringResult< double > const clustering{ distancesPerPointPerCluster };

  EXPECT_EQ( clustering.numberOfClusters(), 4 );

  EXPECT_EQ( clustering.numberOfPointsPerCluster()[0], 3 );
  EXPECT_EQ( clustering.numberOfPointsPerCluster()[1], 2 );
  EXPECT_EQ( clustering.numberOfPointsPerCluster()[2], 1 );
  EXPECT_EQ( clustering.numberOfPointsPerCluster()[3], 5 );

  EXPECT_NEAR( clustering.distancesPerPointPerCluster()[0][0], distancesPerPointPerCluster[0][0], tol );
  EXPECT_NEAR( clustering.distancesPerPointPerCluster()[0][1], distancesPerPointPerCluster[0][1], tol );
  EXPECT_NEAR( clustering.distancesPerPointPerCluster()[0][2], distancesPerPointPerCluster[0][2], tol );
  EXPECT_NEAR( clustering.distancesPerPointPerCluster()[1][0], distancesPerPointPerCluster[1][0], tol );
  EXPECT_NEAR( clustering.distancesPerPointPerCluster()[1][1], distancesPerPointPerCluster[1][1], tol );
  EXPECT_NEAR( clustering.distancesPerPointPerCluster()[2][0], distancesPerPointPerCluster[2][0], tol );
  EXPECT_NEAR( clustering.distancesPerPointPerCluster()[3][0], distancesPerPointPerCluster[3][0], tol );
  EXPECT_NEAR( clustering.distancesPerPointPerCluster()[3][1], distancesPerPointPerCluster[3][1], tol );
  EXPECT_NEAR( clustering.distancesPerPointPerCluster()[3][2], distancesPerPointPerCluster[3][2], tol );
  EXPECT_NEAR( clustering.distancesPerPointPerCluster()[3][3], distancesPerPointPerCluster[3][3], tol );
  EXPECT_NEAR( clustering.distancesPerPointPerCluster()[3][4], distancesPerPointPerCluster[3][4], tol );
}
