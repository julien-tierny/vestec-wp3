Topological Analysis of the Forest Fire Simulator Output
========================================================

This repository gathers various software dependencies needed to apply
Sorbonne Université's topological analysis on Tecnosylva's Forest Fire
data. You will find below a list of steps to reproduce our results.

Software Dependencies
---------------------

To reproduce our results, you will need the following pieces of
software:
* Qt5 library
* ParaView (with TTK patches), for its Catalyst in-situ framework
* TTK, the topological data analysis library
  + ZFP is a numerical compression library, and one non-packaged dependency of TTK
* OSPRay, a portable ray tracing engine used by Paraview

Those dependencies have been put as *git submodules* of the current
repository. To fetch them, please run

```sh
git submodule update --init
```
in the repository root folder.

Other dependencies required by OSPRay are:
* Intel SPMD Program Compiler (ISPC), a compiler variant of the C
  programming language adopting SPMD model
* Intel Embree, a collection of high-performance ray tracing kernels

Eigen, a C++ header-only linear algebra library, is required also by
TTK to run on FBK's data.

Such dependencies are downloaded as binaries directly when running the bash script.


Build (& Install) Everything
----------------------------

You will find here some basic instructions on how to build our
prototype on a Linux machine without the need of sudo rights.
This build process has been tested on SLED12 workstation

This repository is self contained, with the exception of the Numpy C
headers. As such headers cannot be easily installed in a SLED12
system, I relied on conda (miniconda precisely). miniconda can be
downloaded [here](https://docs.conda.io/en/latest/miniconda.html).
Once installed simply execute the following command to install Numpy:

```sh
conda install -y numpy scikit-learn
```
Then, simply run the following command:
```sh
bash make.sh /path/to/numpy/c/headers
```

For some reasons, the TTK configuration pipeline is not able to find
the Numpy headers, and thus, we have to specify them manually in the
corresponding CMake command. Replace `/path/to/numpy/c/headers` with
the path of your local numpy installation.

The script will create two folders: `build` and `install`, containing
the build and install outputs of the libraries compiled during the
process.

#### Notice

Sice Graphviz is optional to VESTEC, this library has been disabled in TTK.

#### Warning

Once enabled OSPRay in ParaView, I got stuck in the following error while compiling Paraview:

```sh
version `GLIBCXX_3.4.26' not found (required by ../../../bin/vtkProcessXML-pv5.7)
```

This issue can be solved by using a newer GCC version. Using the
environment modules, first unload the current GCC (in my case:

```sh
module unload gcc/gcc-8.1.0/sled12.x86_64.gcc.release
```

Then, load the newer GCC 9.2:

```sh
module load gcc/gcc-9.2.0/sled12.x86_64.gcc.release
```

Fetch & analyse the Forest Fire data
------------------------------------

A Python script named `generateAndReducePersistenceDiagrams.py` has
been provided in this repository to perform the following computation steps:

1. download the Forest Fire data from the VESTEC FTP server;
2. compute a persistence diagram from these data and store it in a
   Cinema Database;
4. cluster these persistence diagrams, compute a distance matrix and
   reduce it with MDS to a 3D point cloud. A heat map is also
   generated from the distance matrix.

This Python scripts is used as follows:

* to perform the data fetching step, invoke it through

```bash
python generateAndReducePersistenceDiagrams.py fetch -u USERNAME -p PASSWORD
```

where USERNAME and PASSWORD are the credentials to access the FTP
server. Input data will be stored inside the `input_data` folder.

* to pre-process and generate the persistence diagrams in one step
  (the pre-processed data are generated on the fly and not kept) , use

```bash
pvpython generateAndReducePersistenceDiagrams.py generate
```

(note that this step uses `pvpython` instead of `python` to set some
environment). The persistence diagrams will be stored inside a Cinema
Database named `pdiags.cdb` in the current working directory. An empty
VTK file, `empty.vtu` is also generated alongside as a byproduct.

* to generate the 3D point cloud and the heat map, type

```bash
pvpython generateAndReducePersistenceDiagrams.py cluster
```

This task can take up to several tens of minutes depending on the
available computational power. The results take the form of two VTK
files:

* `fire_distmat.csv` contains the distance matrix;
* `fire_trajectories.vtu` contains the 3D point cloud and
* `fire_heatmap.vtu` contains a heat map of the distance matrix.
