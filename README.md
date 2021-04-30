# VESTEC-WP3: Companion Demonstrator for Deliverable D3.3

## I - Presentation
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



## II - Instructions
### 1. Installation of ParaView and TTK

We will use the 0.9.9 release version of TTK, which is packaged with ParaView 5.8.
Download the packaged binary from the TTK website:
```bash
$ wget https://github.com/topology-tool-kit/ttk/releases/download/0.9.9/ttk-0.9.9-ubuntu-20.04.deb
$ wget https://github.com/topology-tool-kit/ttk-paraview/releases/download/v5.8.1/ttk-paraview-ubuntu-20.04.deb
```
And install the .deb packages, ParaView **then** TTK. It is best to update the
packages information prior to the installation:
```bash
$ sudo apt update
$ sudo apt install ./ttk-paraview-ubuntu-20.04.deb
$ sudo apt install ./ttk-0.9.9-ubuntu-20.04.deb
```

### 2. Clustering of precomputed persistence diagrams
#### With ParaView
Move to the `paraview_states` folder and launch one of the state files to run
the clustering and visualize the result:
```bash
$ cd paraview_states
$ paraview clustering_wildfire_precomputed_data.pvsm
```
The clustering result is display in ParaView (top view). The botton view
presents the corresponding ground truth in a grid layout.

Alternatively, the result of the clustering is printed in the console output:
```bash
[PersistenceDiagramClustering] Clustering result:
[PersistenceDiagramClustering]  Cluster 0 = {0, 1, 2, 3}
[PersistenceDiagramClustering]  Cluster 1 = {8}
[PersistenceDiagramClustering]  Cluster 2 = {4, 5}
[PersistenceDiagramClustering]  Cluster 3 = {6, 7}
[PersistenceDiagramClustering] Complete ...................... [0.281s|12T|100%]
```
The index of the diagrams in each cluster is given by the order of the diagram
in the input cinema database. In can be found in the file 
```
precomputed_diagrams/wildfire/pdiags.cdb/data.csv
```
in the case of the *wildfire* usecase:
```
CaseId,Name,TimeValue,FILE
1,T2,0,data/1_T2_0.vtu
1,T7,2,data/1_T7_2.vtu
1,T8,3,data/1_T8_3.vtu
1,T9,4,data/1_T9_4.vtu
2,T3,5,data/2_T3_5.vtu
2,T10,6,data/2_T10_6.vtu
3,T4,7,data/3_T4_7.vtu
3,T5,8,data/3_T5_8.vtu
0,T1,9,data/0_T1_9.vtu

```

### 3. In-situ pipeline for the Space Weather use case with ipicmini
The `ipicmini` folder contains the source code for the space weather simulations.
#### a. Build the simulator
Move to the `space_weather_in-situ` folder:
```bash
$ cd space_weather_in-situ
```
Install the required dependencies to build `ipicmini`, the simulator interfaced with Catalyst:
```bash
$ sudo apt install cmake libopenmpi-dev
```
Then launch the script `make.sh` in order to build ipicmini:
```bash
$ ./make.sh
```
#### b. Run the program
The `launch_runs.sh` script will run the 4 simulation runs with different
set of input parameters (two different values of the initial magnetic field and
two different ionic populations). For each time step of each simulation, the
persistence diagram of the magnitude of the magnetic field will be computed
and stored in a cinema database, in the `space_weather_in-situ/data/` folder.

Additionally, the script will run the post-processing phase of clustering.

Run the script to generate the results (this will take a long time):
```bash
$ ./launch_runs.sh
```

#### c. Visualize the results

Use ParaView to visualize the results. 

The previous script created a file
`spaceWeather_trajectories.vtu`, where each persistence diagram computed in-situ is represented by a 
point embedded in 3D. The embedding was created by computing the matrix of the
Wasserstein distance between the diagrams, and performing Multi-Dimensional
Scaling. Both operations were realized with TTK.

```bash
$ paraview spaceWeather_trajectories.vtu
```
The four simulation runs can clearly be distinguished. Color the output by
CaseName or ClusterId to see respectively the ground truth classification (1
cluster per run) or the result of the topological clustering on the later
diagrams.
