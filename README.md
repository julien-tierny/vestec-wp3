Topological Analysis and Clustering of the Space Weather Simulator Output
=========================================================================

This repository gathers various software dependencies needed to apply
Sorbonne Université's topological analysis on KTH's Space Weather
simulator. You will find below a list of steps to reproduce our
results.

Software Dependencies
---------------------

To reproduce our results, you will need the following pieces of
software:
* QT5 library
* ParaView (with TTK patches), for its Catalyst in-situ framework
* TTK, the topological data analysis library
  + ZFP is a numerical compression library, and one non-packaged dependency of TTK
* ipicmini, KTH's version of the iPic3D space weather simulator
* OSPRay, a portable ray tracing engine used by Paraview

Those dependencies have been put as *git submodules* of the current
repository. To fetch them, please run

```sh
git submodule update --init
```
in the repository root folder.

Other dependencies required by OSPRay are:
* Intel SPMD Program Compiler (ISPC), a compiler variant of the C programming language adopting SPMD model
* Intel Embree, s a collection of high-performance ray tracing kernels

Such dependencies are downloaded as binaries directly when running the bash script.

Build (& Install) Everything
----------------------------

You will find here some basic instructions on how to build our
prototype on a Linux machine without the need of sudo rights.
This build process has been tested on SLED12 workstation

This repository is self contained, with the exception of the numpy C headers.
As such headers cannot be easily installed in a SLED12 system, I relied on conda (miniconda precisely).
miniconda can be downloaded [here](https://docs.conda.io/en/latest/miniconda.html).
Once installed simply execute the following command to install numpy:

```sh
conda install -y numpy scikit-learn 
```
Then, simply run the following command:
```sh
bash make.sh /path/to/numpy/c/headers
```
For some reasons, the TTK configuration pipeline is not able to find the numpy headers, and thus, we have to specify them manually in the corresponding cmake command. Replace `/path/to/numpy/c/headers` with the path of your local numpy installation.

The script will create two folders: `build` and `install`, containing the build and install outputs of the libraries compiled during the process.

#### Notice
Sice Eigen and Graphviz are optional to VESTEC, these libraries have been disabled in Paraview.
#### Warning
Once enabled OSPRay in Paraview, I got stuck in the following error while compiling Paraview:
```sh
version `GLIBCXX_3.4.26' not found (required by ../../../bin/vtkProcessXML-pv5.7) 
```
This issue can be solved by using a newer gcc version. Using the environmet modules, first unload the current gcc (in my case:
```sh
module unload gcc/gcc-8.1.0/sled12.x86_64.gcc.release
```
Then, load the newer gcc 9.2:
```sh
module load gcc/gcc-9.2.0/sled12.x86_64.gcc.release
```

Run the Simulator & Extract Persistence Diagrams
------------------------------------------------

Once the above build & installation step is completed, the simulator
can be launched with provided in-situ TTK pipelines to extract
persistence diagrams at every simulation cycle:

```sh
cd ipicmini
build/iPICmini inputfiles/GEM3D.inp
```

The simulation parameters can be changed freely in the
`inputfiles/GEM3D.inp` file. I was suggested to change the `ns`
parameter from 4 to 2 and to adapt the initial magnetic field `B0z`
from 0.0 to 0.0195 (same value as `B0x`).

The simulation will generate and store data inside the `data/`
subdirectory, persistence diagrams for the magnitude of the magnetic
field B and the z component Bz in the `pdiags.cdb` Cinema Database
every cycle and topological compression of simulation outputs (every
10 cycles) inside the `tcomp.cdb` Cinema Database. See the Python
Catalyst script `inputfiles/storePersistenceDiagrams.py` for more
information.

Once the simulation has ended (after 2500 cycles), it is possible to
modify the simulation parameters (as described above) to generate more
persistence diagrams.

To reduce the persistence diagrams to a 3D point cloud and cluster the
last persistence diagrams per simulation run, another Python script is
available in the simulator submodule. To run it:

```sh
cd ipicmini
pvpython inputfiles/clusterDiags.py
```

It will generate two files inside the `ipicmini/data/` subdirectory:
* a CSV file named `spaceWeather_distmat.csv` containing (among
  others) the distance matrix between all persistence diagrams
* a VTU file named `spaceWeather_trajectories.vtu` with the point
  cloud generated from the dimension reduction of the distance
  matrix.

The latter represents the trajectory of each simulation run, in the
abstract space of the persistence diagrams. Each point also points to
the file containing the corresponding persistence diagram inside the
`ipicmini/data/pdiags.cdb` Cinema Database. From the simulation
parameters and the TimeStep value, one can also retrieve the
corresponding compressed version inside the `ipicmini/data/tcomp.cdb`
(if TimeStep is divisible by 10).

Further post-processing of this point cloud might include associating
the ttkStringArrayConverter, the Threshold and the ttkPointSetToCurve
filters to manipulate simulation run trajectories instead of
individual points.
