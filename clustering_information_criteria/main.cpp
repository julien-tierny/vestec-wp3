/*! @file main.hpp
 * @brief reads clustering result data from files and compares information criteria
 * @author Max Kontak <Max.Kontak@DLR.de>
 * @date 2019-06-03
 */

#include "clustering_result.hpp"
#include "information_criteria.hpp"
#include "likelihoods.hpp"

#include <fstream>
#include <iostream>
#include <string>
#include <tuple>
#include <vector>

template< typename Scalar >
decltype(auto) clusteringResultFromASCIIFile( std::string & filename )
{
  std::ifstream istream{ filename };
  return clusteringResultFromASCIIStream< Scalar >( istream );
}

int main()
{
  auto const dimension{ 0.5 };

  std::vector< int > const numClusters{ 2, 3, 4, 5, 6, 7, 8, 9, 10 };
  auto const I{ numClusters.size() };

  std::vector< std::string > filenames = { "/localdata1/kont_ma/VESTEC/vestec-wp3/data/seaSurfaceHeight/ssh_100s_k2.txt",
                                           "/localdata1/kont_ma/VESTEC/vestec-wp3/data/seaSurfaceHeight/ssh_100s_k3.txt",
                                           "/localdata1/kont_ma/VESTEC/vestec-wp3/data/seaSurfaceHeight/ssh_100s_k4.txt",
                                           "/localdata1/kont_ma/VESTEC/vestec-wp3/data/seaSurfaceHeight/ssh_100s_k5.txt",
                                           "/localdata1/kont_ma/VESTEC/vestec-wp3/data/seaSurfaceHeight/ssh_100s_k6.txt",
                                           "/localdata1/kont_ma/VESTEC/vestec-wp3/data/seaSurfaceHeight/ssh_100s_k7.txt",
                                           "/localdata1/kont_ma/VESTEC/vestec-wp3/data/seaSurfaceHeight/ssh_100s_k8.txt",
                                           "/localdata1/kont_ma/VESTEC/vestec-wp3/data/seaSurfaceHeight/ssh_100s_k9.txt",
                                           "/localdata1/kont_ma/VESTEC/vestec-wp3/data/seaSurfaceHeight/ssh_100s_k10.txt" };

  std::vector< double > AICValues  ( I );
  std::vector< double > AICcValues ( I );
  std::vector< double > BICValues  ( I );

  for ( int i=0; i<I; ++i )
  {
    auto const clustering{ clusteringResultFromASCIIFile< double >( filenames[i] ) };

    auto const likelihood{ Likelihoods::unitVariance(clustering) };

    decltype(I) const numEffectiveParam = dimension*numClusters[i];
    auto const sampleSize{ clustering.numberOfPoints() };

    AICValues[i]  = AkaikeInformationCriterion          ( likelihood, numEffectiveParam );
    AICcValues[i] = AkaikeInformationCriterionCorrected ( likelihood, numEffectiveParam, sampleSize );
    BICValues[i]  = BayesianInformationCriterion        ( likelihood, numEffectiveParam, sampleSize );
  }

  std::cout << "Clustering with likelihood: residual sum of squares\n\n";
  std::cout << "AIC (normalized wrt value at k=1): \n";
  for (int i=0; i<I; ++i)
  {
    std::cout << "  k=" << numClusters[i] << ": " << AICValues[i]/std::abs(AICValues[0]) << "\n";
  }
  std::cout << std::endl;

  std::cout << "AICc (normalized wrt value at k=1): \n";
  for (int i=0; i<I; ++i)
  {
    std::cout << "  k=" << numClusters[i] << ": " << AICcValues[i]/std::abs(AICcValues[0]) << "\n";
  }
  std::cout << std::endl;

  std::cout << "BIC (normalized wrt value at k=1): \n";
  for (int i=0; i<I; ++i)
  {
    std::cout << "  k=" << numClusters[i] << ": " << BICValues[i]/std::abs(BICValues[0]) << "\n";
  }
  std::cout << std::endl;

  return 0;
}
