/*! @file information_criteria.hpp
 * @brief functionality for information criteria
 * @author Max Kontak <Max.Kontak@DLR.de>
 * @date 2019-05-10
 */

#include <cmath>

/** @brief Compute the Akaike Information Criterion of a statistical model
 **
 **	The Akaike Information Criterion (AIC) for a given estimated statistical model is given by
 **     AIC = 2*k - 2*ln(L),
 ** where k is the number of (effective) parameters and L is the maximum value of the likelihood-function.
 **
 ** @param  maxValLogLikelihood  maximum value of the log-likelihood function of the model
 ** @param  numberOfParameters   number of (effective) parameters of the model
 ** @return  										 value of the Akaike Information Criterion (AIC)
 **
 ** Note that the input of the maximum value of the log-likelihood is required, not of the likelihood itself.
 **/
template< typename Scalar >
Scalar AkaikeInformationCriterion( Scalar maxValLogLikelihood,
                                   int    numberOfParameters )
{
  return 2*Scalar(numberOfParameters) - 2*maxValLogLikelihood;
}



/** @brief Compute the corrected Akaike Information Criterion of a statistical model
 **
 **	The corrected Akaike Information Criterion (AICc) for a given estimated statistical model is given by
 **     AICc = AIC + (2*k^2 + 2*k)/(n - k - 1)
 ** where k is the number of (effective parameters) and n is the number of samples.
 ** In contrast to the AIC, the AICc accounts better for small sample sizes.
 **
 ** @param  maxValLogLikelihood  maximum value of the log-likelihood function of the model
 ** @param  numberOfParameters   number of (effective) parameters of the model
 ** @param  numberOfSamples      number of sample
 ** @return  										 value of the Akaike Information Criterion (AIC)
 **
 ** Note that the input of the maximum value of the log-likelihood is required, not of the likelihood itself.
 **/
template< typename Scalar >
Scalar AkaikeInformationCriterionCorrected( Scalar maxValLogLikelihood,
                                            int    numberOfParameters,
                                            int    numberOfSamples )
{
  return AkaikeInformationCriterion( maxValLogLikelihood, numberOfParameters )
         + ( 2 * Scalar(numberOfParameters) * (Scalar(numberOfParameters) + 1) )
         / (Scalar(numberOfSamples) - Scalar(numberOfParameters) - 1);
}



/** @brief Compute the Bayesioan Information Criterion of a statistical model
 **
 **	The Bayesian Information Criterion (BIC) for a given estimated statistical model is given by
 **     BIC = ln(n)*k - 2*ln(L)
 ** where n is the sample size, k is the number of (effective) parameters and L is the maximum value of the likelihood function.
 ** In contrast to the AIC, the AICc accounts better for small sample sizes.
 **
 ** @param  maxValLogLikelihood  maximum value of the log-likelihood function of the model
 ** @param  numberOfParameters   number of (effective) parameters of the model
 ** @param  numberOfSamples      number of sample
 ** @return  										 value of the Akaike Information Criterion (AIC)
 **
 ** Note that the input of the maximum value of the log-likelihood is required, not of the likelihood itself.
 **/
template< typename Scalar >
Scalar BayesianInformationCriterion( Scalar maxValLogLikelihood,
                                     int    numberOfParameters,
                                     int    numberOfSamples )
{
  return std::log(numberOfSamples) * numberOfParameters - 2*maxValLogLikelihood;
}
