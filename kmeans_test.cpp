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
  std::vector<vec>::size_type const constexpr sampleSizePerCluster{ 1000000 };
  std::vector<vec>::size_type const constexpr maxIter{ 10*maxK };
#else
  std::vector<vec>::size_type const constexpr maxK{ 5 };
  std::vector<vec>::size_type const constexpr sampleSizePerCluster{ 1000 };
  std::vector<vec>::size_type const constexpr maxIter{ 4*maxK };
#endif

  // initialize random number generator
  std::random_device rd{};
  std::mt19937 gen{rd()};

  std::normal_distribution<> gaussian{};

  // data vector

  std::vector< vec > data;
  data.resize(3*sampleSizePerCluster);

  // construct 100 points around (0,0)
  for (auto i=0U*sampleSizePerCluster; i<1U*sampleSizePerCluster; ++i)
  {
    data[i][0] = 0 + gaussian(gen);
    data[i][1] = 0 + gaussian(gen);
  }

  // construct 100 points around (10,0)
  for (auto i=1U*sampleSizePerCluster; i<2U*sampleSizePerCluster; ++i)
  {
    data[i][0] = 10 + gaussian(gen);
    data[i][1] =  0 + gaussian(gen);
  }

  // construct 100 points around (0,10)
  for (auto i=2U*sampleSizePerCluster; i<3U*sampleSizePerCluster; ++i)
  {
    data[i][0] =  0 + gaussian(gen);
    data[i][1] = 10 + gaussian(gen);
  }

  auto const sampleSize{ 3U*sampleSizePerCluster };

  std::vector<ClusteringResult<double>> clusterings{};

  // do clusterings
  for (int k=0; k<maxK; ++k)
  {
    clusterings.emplace_back( KMeans::lloyd_algorithm( data, k+1, maxIter ) );
  }

  {
    // clustering with residual sum of squares as likelihood (assumption: unit variance)
    std::array< double, maxK > likelihoods{};
    std::array< double, maxK > AICValues{};
    std::array< double, maxK > AICcValues{};
    std::array< double, maxK > BICValues{};

    for (size_t k=0; k<maxK; ++k)
    {
      auto const likelihood{ Likelihoods::unitVariance(clusterings[k]) };
      likelihoods[k] = likelihood;

      // Established implementations use "2*#clusters" as effective number of parameters, motivated by the 2 entries per cluster centroid.
      // Looking at the statistical model, this is not totally clear, because another parameter is to which cluster each sample belongs.
      // Nevertheless, it works very well!
      auto const numEffectiveParam{ 2*(k+1) };

      AICValues[k]  = AkaikeInformationCriterion          ( likelihood, numEffectiveParam );
      AICcValues[k] = AkaikeInformationCriterionCorrected ( likelihood, numEffectiveParam, sampleSize );
      BICValues[k]  = BayesianInformationCriterion        ( likelihood, numEffectiveParam, sampleSize );
    }

    std::cout << "Clustering with likelihood: residual sum of squares\n\n";
    std::cout << "AIC (normalized wrt value at k=1): \n";
    for (int k=0; k<maxK; ++k)
    {
      std::cout << "  k=" << k+1 << ": " << AICValues[k]/std::abs(AICValues[0]) << "\n";
    }
    std::cout << std::endl;

    std::cout << "AICc (normalized wrt value at k=1): \n";
    for (int k=0; k<maxK; ++k)
    {
      std::cout << "  k=" << k+1 << ": " << AICcValues[k]/std::abs(AICcValues[0]) << "\n";
    }
    std::cout << std::endl;

    std::cout << "BIC (normalized wrt value at k=1): \n";
    for (int k=0; k<maxK; ++k)
    {
      std::cout << "  k=" << k+1 << ": " << BICValues[k]/std::abs(BICValues[0]) << "\n";
    }
    std::cout << std::endl;
  }

  {
    // clustering with log-likelihood function corresponding to the assumption of same variance per cluster
    std::array< double, maxK > likelihoods{};
    std::array< double, maxK > AICValues{};
    std::array< double, maxK > AICcValues{};
    std::array< double, maxK > BICValues{};

    for (size_t k=0; k<maxK; ++k)
    {
      // this likelihood is based on the assumption that all clusters have the same variance
      auto const likelihood{ Likelihoods::sameVariance(clusterings[k], 2U) };
      likelihoods[k] = likelihood;

      // the number of effective parameters consists of:
      // 2*#Clusters          parameters for the centroids' components
      // #Clusters*#samples   parameters for the binary variables, which encode if a data point belongs to a cluster
      auto const numEffectiveParam{ 2*(k+1) + (k+1)*sampleSize };
      AICValues[k]  = AkaikeInformationCriterion          ( likelihood, numEffectiveParam );
      AICcValues[k] = AkaikeInformationCriterionCorrected ( likelihood, numEffectiveParam, sampleSize );
      BICValues[k]  = BayesianInformationCriterion        ( likelihood, numEffectiveParam, sampleSize );
    }

    std::cout << "Clustering with likelihood: same variance\n\n";
    std::cout << "AIC (normalized wrt value at k=1): \n";
    for (int k=0; k<maxK; ++k)
    {
      std::cout << "  k=" << k+1 << ": " << AICValues[k]/std::abs(AICValues[0]) << "\n";
    }
    std::cout << std::endl;

    std::cout << "AICc (normalized wrt value at k=1): \n";
    for (int k=0; k<maxK; ++k)
    {
      std::cout << "  k=" << k+1 << ": " << AICcValues[k]/std::abs(AICcValues[0]) << "\n";
    }
    std::cout << std::endl;

    std::cout << "BIC (normalized wrt value at k=1): \n";
    for (int k=0; k<maxK; ++k)
    {
      std::cout << "  k=" << k+1 << ": " << BICValues[k]/std::abs(BICValues[0]) << "\n";
    }
    std::cout << std::endl;
  }
}
