/*! @file main.hpp
 * @brief reads clustering result data from files and compares information criteria
 * @author Max Kontak <Max.Kontak@DLR.de>
 * @date 2019-06-03
 */

#include "clustering_result.hpp"
#include "information_criteria.hpp"
#include "likelihoods.hpp"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <numeric>
#include <stdexcept>
#include <string>
#include <tuple>
#include <vector>

template< typename Scalar >
decltype(auto) clusteringResultFromASCIIFile( std::string & filename )
{
  std::ifstream istream{ filename };
  return clusteringResultFromASCIIStream< Scalar >( istream );
}

template< typename T1, typename T2 >
decltype(auto) sortVectorByOtherVector( std::vector<T1> const & toSort,
                                        std::vector<T2> const & sortBy )
{
  using index = typename std::vector<T1>::size_type;

  if ( toSort.size() != sortBy.size() )
    throw std::invalid_argument("Both inputs to sortVectorByOtherVector have to be of the same size.");

  auto const N = toSort.size();

  std::vector< index > sortedIndices;
  sortedIndices.resize( N );

  // fill vector with 0, ..., size-1
  std::iota( sortedIndices.begin(), sortedIndices.end(), 0 );

  // sort indices by values in sortBy
  std::sort( sortedIndices.begin(), sortedIndices.end(), [&sortBy]( auto const i, auto const j ){return sortBy[i]<sortBy[j];} );

  std::vector<T1> outputVector;
  outputVector.resize( N );

  for (index i=0; i<N; ++i)
  {
    outputVector[i] = toSort[sortedIndices[i]];
  }

  return outputVector;
}

void seaSurfaceHeight()
{
  auto const dimension{ 1 };

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

  std::vector< double > likelihoods( I );
  std::vector< double > AICValues  ( I );
  std::vector< double > AICcValues ( I );
  std::vector< double > BICValues  ( I );

  for ( int i=0; i<I; ++i )
  {
    auto const clustering{ clusteringResultFromASCIIFile< double >( filenames[i] ) };

    likelihoods[i] = Likelihoods::sameVariance(clustering, dimension);

    decltype(I) const numEffectiveParam = dimension*numClusters[i];
    auto const sampleSize{ clustering.numberOfPoints() };

    AICValues[i]  = AkaikeInformationCriterion          ( likelihoods[i], numEffectiveParam );
    AICcValues[i] = AkaikeInformationCriterionCorrected ( likelihoods[i], numEffectiveParam, sampleSize );
    BICValues[i]  = BayesianInformationCriterion        ( likelihoods[i], numEffectiveParam, sampleSize );
  }

  std::cout << "Clustering with likelihood: unitVariance\n\n";
  std::cout << "AIC (normalized wrt value at k=2): \n";
  for (int i=0; i<I; ++i)
  {
    std::cout << "  k=" << numClusters[i] << ": " << AICValues[i]/std::abs(AICValues[0]) << "\n";
  }
  std::cout << std::endl;

  auto const sortedNumClustersAIC{ sortVectorByOtherVector( numClusters, AICValues )};
  std::cout << "k sorted by AIC value: ";
  for ( auto v : sortedNumClustersAIC )
    std::cout << v << ", ";

  std::cout << std::endl << std::endl;

  std::cout << "AICc (normalized wrt value at k=2): \n";
  for (int i=0; i<I; ++i)
  {
    std::cout << "  k=" << numClusters[i] << ": " << AICcValues[i]/std::abs(AICcValues[0]) << "\n";
  }
  std::cout << std::endl;

  auto const sortedNumClustersAICc{ sortVectorByOtherVector( numClusters, AICcValues )};
  std::cout << "k sorted by AICc value: ";
  for ( auto v : sortedNumClustersAICc )
    std::cout << v << ", ";

  std::cout << std::endl << std::endl;

  std::cout << "BIC (normalized wrt value at k=2): \n";
  for (int i=0; i<I; ++i)
  {
    std::cout << "  k=" << numClusters[i] << ": " << BICValues[i]/std::abs(BICValues[0]) << "\n";
  }
  std::cout << std::endl;

  auto const sortedNumClustersBIC{ sortVectorByOtherVector( numClusters, BICValues )};
  std::cout << "k sorted by BIC value: ";
  for ( auto v : sortedNumClustersBIC )
    std::cout << v << ", ";

  std::cout << std::endl << std::endl;
}

void isabel()
{
  auto const dimension{ 0.5 };

  std::vector< int > const numClusters{ 2, 3, 4, 5, 6, 7, 8, 9, 10 };
  auto const I{ numClusters.size() };

  std::vector< std::string > filenames = { "/localdata1/kont_ma/VESTEC/vestec-wp3/data/isabel/isabel_100s_k2.txt",
                                           "/localdata1/kont_ma/VESTEC/vestec-wp3/data/isabel/isabel_100s_k3.txt",
                                           "/localdata1/kont_ma/VESTEC/vestec-wp3/data/isabel/isabel_100s_k4.txt",
                                           "/localdata1/kont_ma/VESTEC/vestec-wp3/data/isabel/isabel_100s_k5.txt",
                                           "/localdata1/kont_ma/VESTEC/vestec-wp3/data/isabel/isabel_100s_k6.txt",
                                           "/localdata1/kont_ma/VESTEC/vestec-wp3/data/isabel/isabel_100s_k7.txt",
                                           "/localdata1/kont_ma/VESTEC/vestec-wp3/data/isabel/isabel_100s_k8.txt",
                                           "/localdata1/kont_ma/VESTEC/vestec-wp3/data/isabel/isabel_100s_k9.txt",
                                           "/localdata1/kont_ma/VESTEC/vestec-wp3/data/isabel/isabel_100s_k10.txt" };

  std::vector< double > AICValues  ( I );
  std::vector< double > AICcValues ( I );
  std::vector< double > BICValues  ( I );

  for ( int i=0; i<I; ++i )
  {
    auto const clustering{ clusteringResultFromASCIIFile< double >( filenames[i] ) };

    auto const likelihood{ Likelihoods::sameVariance(clustering, dimension) };

    decltype(I) const numEffectiveParam = dimension*numClusters[i];
    auto const sampleSize{ clustering.numberOfPoints() };

    AICValues[i]  = AkaikeInformationCriterion          ( likelihood, numEffectiveParam );
    AICcValues[i] = AkaikeInformationCriterionCorrected ( likelihood, numEffectiveParam, sampleSize );
    BICValues[i]  = BayesianInformationCriterion        ( likelihood, numEffectiveParam, sampleSize );
  }

  std::cout << "Clustering with likelihood: unitVariance\n\n";
  std::cout << "AIC (normalized wrt value at k=2): \n";
  for (int i=0; i<I; ++i)
  {
    std::cout << "  k=" << numClusters[i] << ": " << AICValues[i]/std::abs(AICValues[0]) << "\n";
  }
  std::cout << std::endl;

  auto const sortedNumClustersAIC{ sortVectorByOtherVector( numClusters, AICValues )};
  std::cout << "k sorted by AIC value: ";
  for ( auto v : sortedNumClustersAIC )
    std::cout << v << ", ";

  std::cout << std::endl << std::endl;

  std::cout << "AICc (normalized wrt value at k=2): \n";
  for (int i=0; i<I; ++i)
  {
    std::cout << "  k=" << numClusters[i] << ": " << AICcValues[i]/std::abs(AICcValues[0]) << "\n";
  }
  std::cout << std::endl;

  auto const sortedNumClustersAICc{ sortVectorByOtherVector( numClusters, AICcValues )};
  std::cout << "k sorted by AICc value: ";
  for ( auto v : sortedNumClustersAICc )
    std::cout << v << ", ";

  std::cout << std::endl << std::endl;

  std::cout << "BIC (normalized wrt value at k=2): \n";
  for (int i=0; i<I; ++i)
  {
    std::cout << "  k=" << numClusters[i] << ": " << BICValues[i]/std::abs(BICValues[0]) << "\n";
  }
  std::cout << std::endl;

  auto const sortedNumClustersBIC{ sortVectorByOtherVector( numClusters, BICValues )};
  std::cout << "k sorted by BIC value: ";
  for ( auto v : sortedNumClustersBIC )
    std::cout << v << ", ";

  std::cout << std::endl << std::endl;
}

int main()
{
  std::cout << "sea surface height data set:\n";
  std::cout << "============================";
  std::cout << "expected result: 4 clusters\n";
  seaSurfaceHeight();
  std::cout << std::endl;

  std::cout << "isabel data set:\n";
  std::cout << "================";
  std::cout << "expected result: 3 clusters\n";
  isabel();
}
