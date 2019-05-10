/*! @file information_criteria_test.cpp
 * @brief test functionality for information criteria
 * @author Max Kontak <Max.Kontak@DLR.de>
 * @date 2019-05-10
 */

#include "information_criteria.hpp"

#include <gtest/gtest.h>

#include <cmath>
#include <limits>

auto const tol{ std::sqrt(std::numeric_limits<double>::epsilon()) };

TEST( InformationCriteria, AkaikeInformationCriterion )
{
  EXPECT_NEAR( AkaikeInformationCriterion(  0.0,  0 ),   0.0, tol );

  EXPECT_NEAR( AkaikeInformationCriterion(  0.0,  1 ),   2.0, tol );
  EXPECT_NEAR( AkaikeInformationCriterion(  0.0, 10 ),  20.0, tol );

  EXPECT_NEAR( AkaikeInformationCriterion(  1.0,  0 ),  -2.0, tol );
  EXPECT_NEAR( AkaikeInformationCriterion( 10.0,  0 ), -20.0, tol );

  EXPECT_NEAR( AkaikeInformationCriterion(  1.0, 10 ),  18.0, tol );
  EXPECT_NEAR( AkaikeInformationCriterion( 10.0,  1 ), -18.0, tol );
}



TEST( InformationCriteria, AkaikeInformationCriterionCorrected )
{
  EXPECT_NEAR( AkaikeInformationCriterionCorrected(   0.0,  0,  0 ),    0.0, tol );

  EXPECT_NEAR( AkaikeInformationCriterionCorrected(   0.0,  1,  0 ),    0.0, tol );
  EXPECT_NEAR( AkaikeInformationCriterionCorrected(   0.0, 10,  0 ),    0.0, tol );

  EXPECT_NEAR( AkaikeInformationCriterionCorrected(   1.0,  0,  0 ),   -2.0, tol );
  EXPECT_NEAR( AkaikeInformationCriterionCorrected(  10.0,  0,  0 ),  -20.0, tol );

  EXPECT_NEAR( AkaikeInformationCriterionCorrected(   1.0, 10,  0 ),   -2.0, tol );
  EXPECT_NEAR( AkaikeInformationCriterionCorrected(  10.0,  1,  0 ),  -20.0, tol );

  EXPECT_NEAR (AkaikeInformationCriterionCorrected( 100.0,  1, 10 ), -197.5, tol );
}



TEST( InformationCriteria, BayesianInformationCriterion )
{
  EXPECT_NEAR( BayesianInformationCriterion(   0.0,  0,  1 ),    0.0,           tol );

  EXPECT_NEAR( BayesianInformationCriterion(   0.0,  1,  2 ),    0.69314718,    tol );
  EXPECT_NEAR( BayesianInformationCriterion(   0.0, 10,  2 ),    6.9314718,     tol );

  EXPECT_NEAR( BayesianInformationCriterion(   1.0,  0,  3 ),   -2.0,           tol );
  EXPECT_NEAR( BayesianInformationCriterion(  10.0,  0,  3 ),  -20.0,           tol );

  EXPECT_NEAR( BayesianInformationCriterion(   1.0, 10,  4 ),   11.8629436,     tol );
  EXPECT_NEAR( BayesianInformationCriterion(  10.0,  1,  4 ),  -18.61370563888, tol );

  EXPECT_NEAR (BayesianInformationCriterion( 100.0,  5, 10 ), -188.487074535,   tol );
}



int main( int argc, char **argv )
{
  testing::InitGoogleTest( &argc, argv );

  return RUN_ALL_TESTS();
}
