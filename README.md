# VESTEC-WP3: Companion Demonstrator for Deliverable D3.3

## Presentation
This repository constitutes the companion demonstrator for the deliverable
D3.3: Clustering of Topological Proxies. 

It contains the data and instructions needed to perform a topological clustering of three
ensemble data sets from the three VESTEC use cases
(*wildfire*, *mosquito-borne disease*, and *space weather*) resulting from
numerical simulations.
Specifically, the purpose of our topological clustering algorithm is to
operate on an ensemble of persistence diagrams, computed in an **in-situ**
fashion during the numerical simulation.

### Data
The data was thoroughly described in the deliverable D3.3.

The *wildfire* data set contains 9 members, classified in 4 clusters based on
the shape of the fire.
The *mosquito-borne disease* data set contains 20 members, classified in 5
clusters based on the simulated year.
The *space weather* data set contains 20 members, classified in 4 clusters
based on the input set of parameters of the corresponding simuation run.

### Clustering on precomputed data
Precomputed persistence diagrams for the three use cases are available in the
`precomputed_diagrams` folder and are stored in cinema databases.

Below are instructions to apply our clustering method to these three precomputed ensemble of persistence diagrams.
The clustering is performed with the Topology Toolkit (TTK: https://topology-tool-kit.github.io/) used as a ParaView plugin.
We demonstrate two ways of using our clustering module with TTK: inside
ParaView and with a pvpython script.
The `paraview_states` folder contains three ParaView state files that can be
used to launch the clustering algorithm and visualize the results in ParaView.
The `clustering_scripts` folder contains pvpython scripts to perform the
clustering and save the results as vtk files.

### In-situ computation of persistence diagrams
Additionally, we provide a demonstration of the complete in-situ topological
analysis pipeline for the *space weather* use case. In this example, we run four
numerical simulations with different input set of parameters. The persistence
diagram of the magnitude of the resulting magnetic field B is computed
**in-situ** using Catalyst, at
each one of the 2500 time steps for each simulation.
In a post-processing step, persistence diagrams from the later time
steps are clustered using our progressive algorithm. 
Persistence diagrams from the whole simulation are also reduced to a 3D point
cloud using Multi-Dimensional Scaling (MDS) on the Wasserstein distance matrix
between the members of the whole ensemble.

## Installation of TTK and ParaView

## Clustering of precomputed persistence diagrams

## In-situ pipeline for the Space Weather use case with ipicmini
