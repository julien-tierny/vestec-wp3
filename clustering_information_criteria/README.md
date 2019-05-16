# Information Criteria for Clustering

This code provides the computation of so-called information criteria for clustering (e.g., k-means clustering).
These information criteria can then be used to determine a suited value for the number of clusters.

# The `ClusteringResult` class

An object of the `ClusteringResult` class needs to be returned from the applied clustering code.
The only information passed to the constructor of this class is a `std::vector<std::vector<Scalar>>`, which holds for each cluster a vector of distances of the points in that cluster to the cluster's centroid.

# The Information Criteria

To derive information criteria, one interprets clustering as a statistcal model, in particular, a mixture of Gaußian variables.

All information criteria are of the form `IC = 2*P - 2*log(L)`, where `P` is a penalty term for the number of parameters of the model and `L` is the value of the likelihood function at the estimated parameters.
Evaluating the information criteria for different numbers of clusters `k` and finding the minimum value yields the ideal number of clusters.
This can be seen as a weighted minimization of the goodness of fit (log-likelihood) and the number of parameters.

Specific choices for `P` lead to the *Akaike Information criterion (AIC)*, the *corrected Akaike Information Criterion (AICc)*, and the *Bayesian Information Criterion (BIC)*.

# The `Likelihoods`

There are different approaches for the `log(L)` term in the information criteria, which are derived under different assumptions on the statistical model.
We provide two implementations:
  * `unitVariance`: one assumes that the variance in all clusters is 1. This, basically, yields the residual sum of squares (RSS) as the log-likelihood.
  * `sameVariance`: assuming that all clusters have the same variance (which might be different from 1), yields a different likelihood function.

# K-means clustering

A basic k-means implementation is provided to be able to test the optimization of information criteria.

# How to compile and test

Use the following commands to compile both the application itself as well as the unit tests:
```
  mkdir build
  cd build
  cmake ..
  make
```
The CMake command should automatically clone the googletest repository into the build folder.

The unit tests can be executed by the command
`./clustering_ic_test`
in the `build/` directory.

# The KMeans test case

The optimization of information criteria for k-means clustering is tested in the `KMeans` test case.
A test data set consisting of three clusters is created and k-means clustering is performed on this data set.
We observe for both types of likelihoods that k=3 has the minimum information criteria value for most of the different criteria.
