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
* ParaView (with TTK patches), for its Catalyst in-situ framework
* TTK, the topological data analysis library
  + ZFP is a numerical compression library, and one non-packaged dependency of TTK
* ipicmini, KTH's version of the iPic3D space weather simulator

Those dependencies have been put as *git submodules* of the current
repository. To fetch them, please run

```sh
git submodule update --init
```
in the repository root folder.


Build (& Install) Everything
----------------------------

You will find here some basic instructions on how to build our
prototype on a Linux machine.


Just follow these instructions and hopefully everything
will work fine!

### 1. ParaView

Since TTK and ipicmini depend on ParaView, you will have to build it
first. A pre-patched ParaView 5.7.0 has been embedded as a git
submodule.

First install the set of ParaView build-time dependencies. Below are
the Ubuntu Bionic ones.

```sh
sudo apt-get build-dep paraview
# or alternatively here is a (hopefully full) explicit list
sudo apt install -y \
     cmake-curses-gui \
     g++ \
     libegl1-mesa-dev \
     libpython3-dev \
     libqt5x11extras5-dev \
     libxt-dev \
     qt5-default \
     qttools5-dev \
     qtxmlpatterns5-dev-tools \
```

After that, let's build and install ParaView:

```sh
cd paraview-ttk
mkdir build
cd build
cmake ..
cmake --build .        # or your default generator build command,
                       # generally "make" or "ninja". You can reduce
                       # the build time by adding " -j$(nproc)"
sudo cmake --install . # for CMake >=3.15. Otherwise, use your default
                       # generator install command (something like
                       # "sudo make install" or "sudo ninja install")
```

Once installed (by default in `$prefix=/usr/local`), make sure that
the `$PATH`, the `$LD_LIBRARY_PATH` and the `$PYTHONPATH` environment
variables point the installation directories (resp. `$prefix/bin`,
`$prefix/lib` and `$prefix/lib/python`major`.`minor`/site-packages`).

### 2. TTK

First install the TTK packaged dependencies. We basically need:

* Boost
* SQLite
* Scikit-Learn
* Zlib
* ZFP (to be installed apart)
* Graphviz (optional for VESTEC)
* Eigen (optional for VESTEC)

Here is a Ubuntu Bionic bash install command:

```sh
sudo apt install -y \
     libboost-dev \
     libeigen3-dev \
     libgraphviz-dev \
     libsqlite3-dev \
     python3-sklearn \
     zlib1g-dev
```

ZFP is one of TTK dependencies that is sadly not (yet) packaged for
Ubuntu. A git submodule containing ZFP is available in this
repository, just build & install it:

```sh
cd zfp
mkdir build
cd build
cmake ..
cmake --build .  # or "make|ninja -j$(nproc)"
sudo cmake --install . # or "sudo make|ninja install"
```

The next step is to build TTK. Since ParaView and ZFP are installed in
the default CMake prefix, TTK should be able to automatically get the
dependencies information.

```sh
cd ttk
mkdir build
cd build
cmake \
      -DTTK_BUILD_STANDALONE_APPS=OFF `# faster compile time` \
      -DTTK_ENABLE_SCIKIT_LEARN=ON \
      -DTTK_ENABLE_ZFP=ON \
      ..
cmake --build . # or "make|ninja -j$(nproc)"
sudo cmake --install . # or "sudo make|ninja install"
```

Since ParaView v5.7.0, you need to set the `$PV_PLUGIN_PATH` environment
variable to point to the TTK ParaView plugin path. Should be something
like:

```sh
export PV_PLUGIN_PATH=$prefix/lib/paraview-5.7/plugins/TopologyToolKit
```

Replace `$prefix` with your default CMake installation prefix,
`/usr/local` by default.

TTK also comes with its own Python script for interfacing with
Scikit-Learn. This script will be installed in
`$prefix/share/scripts/ttk` and this folder should be appended to the
`$PYTHONPATH` environment variable.

Once TTK is installed, you should be able to launch ParaView and
select one out of the three example visualization pipelines from the
welcome screen. Those pipelines should execute without error.


### 3. ipicmini

KTH's Space Weather simulator embedds a Catalyst Adaptor to launch
in-situ ParaView pipelines. It also uses CMake as a build system but
beware: the entry point is in the `src/` subdirectory.

You will need to install a MPI library first, such as OpenMPI or
MPICH.

```sh
cd ipicmini
mkdir build
cd build
cmake \
    -DUSE_CATALYST=ON \
    -DCMAKE_BUILD_TYPE=Release \
    ../src
cmake --build . # or ...
# no need to install it
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
