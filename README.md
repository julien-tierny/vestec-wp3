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
mpirun \
    -np 1 `# only one MPI process` \
    build/iPICmini \
    inputfiles/TaylorGreen.inp # input parameters file
```

The simulation parameters can be changed freely in the
`inputfiles/TaylorGreen.inp` file. I was suggested to change the
`Case` parameter from TaylorGreen{0,1,2,3} and to adapt the
initial magnetic field `B0x` to 0.0185 for TaylorGreen0 to
TaylorGreen2 (keep it a 0.032 for TaylorGreen3).

Once the simulator is launched and after loading the TTK libraries
(~1min), it will launch the simulation and store inside the `data/`
subdirectory the persistence diagrams in the `pdiags.cdb` Cinema
database. A topological compression of the simulation output is also
stored (every 10 cycles) inside the `tcomp.cdb` Cinema database in the
`data/` directory. See the Python Catalyst script
`inputfiles/storePersistenceDiagrams.py` for more information.

Once the simulation has ended (after 2000 cycles), another pipeline is
launched to cluster the persistence diagrams and store them in a
2D point cloud VTU: see `inputfiles/clusterDiags.py`.

Test the clustering on pre-computed inputs
------------------------------------------

Some persistence diagrams have been pre-computed and uploaded onto the
[VESTEC FTP server](ftp://ftp.dlr.de/datasets/persistence_diagrams/).
The Space Weather simulator has been run for 200 cycles four times,
one for every TaylorGreen case parameter (from TaylorGreen0 to
TaylorGreen3).
 The two Cinema databases `pdiags.cdb` and `tcomp.cdb`
contain respectively:
* persistence diagrams taken every 10 cycles and
* topological compressed outputs (using ZFP) taken every 50 cycles.

The ParaView state file `clusterDiags.pvsm` at the root of this
repository can be used to cluster those pre-computed data. Just fetch
the two Cinema databases, put them in the same folder as the state
file and run the pipeline with ParaView.

This ParaView state file opens four layouts representing the following
steps in our analysis pipeline:

1. the first layout represents, on the left, the clustering of the
   persistence diagrams in four clusters, disposed in circles around
   the cluster centroid. The right panel should display the distance
   matrix between those diagrams with the distance metric used for the
   clustering.
2. in the second layout, two identical point clouds with a different
   coloring show the diagrams distance matrix reduced into a 2D
   plane. On the left, the coloring represents the four different
   parameter sets (TaylorGreen0 to TaylorGreen3) used for the four
   simulation runs. On the right, the attached cluster id.
3. in the third panel, we selected three points in the point cloud. On
   the right panels, we show the corresponding persistence diagrams
   and (if available) we load the compressed simulation output.
4. the last layout is a display of the content of our two Cinema
   databases: on the left, the persistence diagrams, on the right, the
   compressed outputs.
