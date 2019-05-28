# Data information

The data folder features the clustering results for the persistence diagrams
of several ensemble data-sets, with different time constraints and number of
clusters.

The seaSurfaceHeight data-set contains 48 ensemble members, divided in 4
clusters. 
The isabel data-set contains 12 ensemble members, divided in 3
clusters.

Each file contains the values for the corresponding
`distancesPerPointPerCluster` object : a vector of vectors of the final
distances of the points in a cluster to the corresponding centroid.
`distancesPerPointPerCluster[i][j]`  is the distance of the j'th point in the
i'th cluster to that cluster's centroid. The vector is printed column by
column and line by line in each file : 

``` distancesPerPointPerCluster[0][0]
distancesPerPointPerCluster[0][1]  ... distancesPerPointPerCluster[1][0]  ...
... 
``` 

The results are provided for each data-set with the number of clusters
`k` varying from 1 to 10, and with time constraints set to 10 and 100 seconds.

As the computation of distances between persistence diagrams is
computationnaly prohibitive, only approximations of those distances are
provided in the data files. During the computation, bounds on the distances
are used to determine the right cluster for each input diagram (Accelerated
KMeans algorithm). Those bounds are updated to the correct distances in a lazy
way, when the approximation cannot be sufficient to conclude on the right
clustering of the data. As such, only the mean of the upper and lower bounds
on the distance is provided in the data files.

Those approximations are likely to be precise in the case of the
seaSurfaceHeight data-set, where the clustering evolves a lot during the
computation. For the isabel data-set, the clustering does not change much
during the process, and the resulting distances are probably a bad
approximation.

