/*! @file kmeans_test.cpp
 * @brief test functionality of kmeans
 * @author Max Kontak <Max.Kontak@DLR.de>
 * @date 2019-05-13
 */

#include "information_criteria.hpp"
#include "kmeans.hpp"
#include "likelihoods.hpp"

#include <gtest/gtest.h>

#include <cmath>
#include <limits>
#include <random>

auto const tol{ std::sqrt(std::numeric_limits<double>::epsilon()) };

TEST( KMeans, lloyd_algorithm )
{
  // this test generates 3 clusters by sampling from a 2D-normal distribution
  // we then apply k-means clustering and compute the AIC, AICc, and BIC to find the optimal k
  using vec = KMeans::vec<double>;

#ifdef NDEBUG
  // larger numbers in release mode
  std::vector<vec>::size_type const constexpr maxK{ 10 };
  std::vector<vec>::size_type const constexpr sampleSize{ 1000000 };
  std::vector<vec>::size_type const constexpr maxIter{ 10*maxK };
#else
  std::vector<vec>::size_type const constexpr maxK{ 5 };
  std::vector<vec>::size_type const constexpr sampleSize{ 1000 };
  std::vector<vec>::size_type const constexpr maxIter{ 4*maxK };
#endif

  // initialize random number generator
  std::random_device rd{};
  std::mt19937 gen{rd()};

  std::normal_distribution<> gaussian{};

  // data vector

  std::vector< vec > data;
  data.resize(3*sampleSize);

  // construct 100 points around (0,0)
  for (auto i=0U*sampleSize; i<1U*sampleSize; ++i)
  {
    data[i][0] = 0 + gaussian(gen);
    data[i][1] = 0 + gaussian(gen);
  }

  // construct 100 points around (10,0)
  for (auto i=1U*sampleSize; i<2U*sampleSize; ++i)
  {
    data[i][0] = 10 + gaussian(gen);
    data[i][1] =  0 + gaussian(gen);
  }

  // construct 100 points around (0,10)
  for (auto i=2U*sampleSize; i<3U*sampleSize; ++i)
  {
    data[i][0] =  0 + gaussian(gen);
    data[i][1] = 10 + gaussian(gen);
  }

  std::array< double, maxK+1 > likelihoods{};
  std::array< double, maxK+1 > AICValues{};
  std::array< double, maxK+1 > AICcValues{};
  std::array< double, maxK+1 > BICValues{};

  for (int k=1; k<=maxK; ++k)
  {
    auto const clustering{ KMeans::lloyd_algorithm( data, k, maxIter ) };
    auto const likelihood{ Likelihoods::residualSumOfSquares(clustering) };
    likelihoods[k] = likelihood;

    // for clustering, we have 2*k effective parameters: the two coordinates of the k centroids
    AICValues[k] = AkaikeInformationCriterion( likelihood, 2*k );
    AICcValues[k] = AkaikeInformationCriterionCorrected( likelihood, 2*k, 3*sampleSize );
    BICValues[k] = BayesianInformationCriterion( likelihood, 2*k, 3*sampleSize );

    std::cout << "";
  }

  std::cout << "AIC: \n";
  for (int k=1; k<=maxK; ++k)
  {
    std::cout << "  k=" << k << ": " << AICValues[k] << "\n";
  }
  std::cout << std::endl;

  std::cout << "AICc: \n";
  for (int k=1; k<=maxK; ++k)
  {
    std::cout << "  k=" << k << ": " << AICcValues[k] << "\n";
  }
  std::cout << std::endl;

  std::cout << "BIC: \n";
  for (int k=1; k<=maxK; ++k)
  {
    std::cout << "  k=" << k << ": " << BICValues[k] << "\n";
  }
  std::cout << std::endl;
}
