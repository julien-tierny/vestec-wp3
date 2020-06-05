#!/bin/bash

# prerequisites:
#     git clone git@github.com:julien-tierny/vestec-wp3.git
#     git checkout space_weather_prototype_DLR_config
#     git submodule update --init

# then, execute this script in the folder containing the repository and submodules..

NUMPY_DIR=$1
if [ "$NUMPY_DIR" == "" ]; then
   echo "[error] the directory containing numpy C headers has to be given as argument! exiting..."
   exit 1;
fi

# Get the current directory - this is the default location for the build and install directory.
CURRENT_DIR="$(pwd)"

# number of build jobs (passed to make -j)
NUM_BUILD_JOBS="$(nproc)"

INSTALL_DIR="$CURRENT_DIR/install"
BUILD_DIR="$CURRENT_DIR/build"


### 0 CREATE BUILD AND INSTALL DIRECTORIES
mkdir {build,install,build/qt5,install/qt5,install/paraview-ttk,build/paraview-ttk}
mkdir {install/zfp,build/zfp,install/ttk,build/ttk,build/ipicmini}
mkdir {build/ospray,install/ospray}


### 1 COMPILE QT
## (INFO: to understand the available flags that you can set type ./configure --help)
cd $CURRENT_DIR/qt5
#git checkout 5.14
#perl init-repository -f
#### we initialize only the modules needed by paraview
perl init-repository -f --module-subset=essential,qtx11extras
#perl init-repository -f --module-subset=default,qtx11extras,-qtwebengine
cd $BUILD_DIR/qt5
../../qt5/configure -prefix "$INSTALL_DIR/qt5" -opensource -confirm-license -nomake tests -nomake examples -silent -release
gmake "-j$NUM_BUILD_JOBS"
gmake install


### 2 INSTALL OSPRAY
cd $CURRENT_DIR
### 2a DOWNLOAD ISPC COMPILER
wget https://sourceforge.net/projects/ispcmirror/files/v1.12.0/ispc-v1.12.0-linux.tar.gz
tar xvzf ispc-v1.12.0-linux.tar.gz
mv ispc-v1.12.0-linux/ ispc/
rm tar xvzf ispc-v1.12.0-linux.tar.gz
### 2b DOWNLOAD EMBREE LIBRARY
wget https://github.com/embree/embree/releases/download/v3.8.0/embree-3.8.0.x86_64.linux.tar.gz
tar xzf embree-3.8.0.x86_64.linux.tar.gz
mv embree-3.8.0.x86_64.linux/ embree/
source embree/embree-vars.sh
rm embree-3.8.0.x86_64.linux.tar.gz
### 2c COMPILE OSPRAY
#cd $CURRENT_DIR/ospray
#git checkout v1.8.5
cd $BUILD_DIR/ospray
#cmake -DCMAKE_INSTALL_PREFIX=/localdata2/VESTEC/vestec-wp3/install/ospray -DBUILD_JOBS=20 /localdata2/VESTEC/vestec-wp3/ospray/scripts/superbuild/
#cmake -DCMAKE_INSTALL_PREFIX=/localdata2/VESTEC/vestec-wp3/install/ospray /localdata2/VESTEC/vestec-wp3/ospray/ -DOSPRAY_TASKING_SYSTEM=OpenMP -Dembree_DIR:PATH=/localdata2/VESTEC/vestec-wp3/embree -DISPC_EXECUTABLE:PATH=/localdata2/VESTEC/vestec-wp3/ispc/bin/ispc
cmake -DCMAKE_INSTALL_PREFIX=$INSTALL_DIR/ospray \
	-Dembree_DIR:PATH=$CURRENT_DIR/embree \
	-DISPC_EXECUTABLE:PATH=$CURRENT_DIR/ispc/bin/ispc \
	-DOSPRAY_TASKING_SYSTEM=OpenMP \
	$CURRENT_DIR/ospray/
cmake --build . --target install --parallel "$NUM_BUILD_JOBS"


### 3 COMPILE PARAVIEW
cd $BUILD_DIR/paraview-ttk
#cmake -DCMAKE_INSTALL_PREFIX=$INSTALL_DIR/paraview-ttk -DQt5_DIR:PATH=$INSTALL_DIR/qt5/lib/cmake/Qt5 -DPARAVIEW_USE_RAYTRACING=OFF -DCMAKE_BUILD_TYPE=Release $CURRENT_DIR/paraview-ttk/
cmake -DCMAKE_INSTALL_PREFIX=$INSTALL_DIR/paraview-ttk \
	-DQt5_DIR:PATH=$INSTALL_DIR/qt5/lib/cmake/Qt5 \
	-Dospray_DIR:PATH=$INSTALL_DIR/ospray/lib64/cmake/ospray-1.8.5 \
	-Dembree_DIR:PATH=$CURRENT_DIR/embree \
	-DCMAKE_BUILD_TYPE=Release \
	$CURRENT_DIR/paraview-ttk/
#cmake -DCMAKE_INSTALL_PREFIX=/localdata2/VESTEC/vestec-wp3/install/paraview-ttk -DQt5_DIR:PATH=/localdata2/VESTEC/vestec-wp3/install/qt5/lib/cmake/Qt5 -Dospray_DIR:PATH=/localdata2/VESTEC/vestec-wp3/install/ospray/lib64/cmake/ospray-1.8.5/ -Dembree_DIR:PATH=/localdata2/VESTEC/vestec-wp3/embree -DCMAKE_BUILD_TYPE=Release /localdata2/VESTEC/vestec-wp3/paraview-ttk/
cmake --build . --target install --parallel "$NUM_BUILD_JOBS"


### 4 COMPILE ZFP
# cd zfp
cd $BUILD_DIR/zfp
cmake -DCMAKE_INSTALL_PREFIX=$INSTALL_DIR/zfp \
	$CURRENT_DIR/zfp
cmake --build . --target install --parallel "$NUM_BUILD_JOBS"


### 5 FETCH EIGEN AND SPECTRA
wget https://gitlab.com/libeigen/eigen/-/archive/3.3.7/eigen-3.3.7.tar.gz
tar xzf eigen-3.3.7.tar.gz
mv eigen-3.3.7/ eigen/
rm eigen-3.3.7.tar.gz
wget https://github.com/yixuan/spectra/archive/v0.9.0.tar.gz
tar xzf v0.9.0.tar.gz
mv spectra-0.9.0/ spectra/
rm v0.9.0.tar.gz


### 6 COMPILE TTK
cd $BUILD_DIR/ttk
cmake DCMAKE_C_FLAGS="-luuid" \
	-DCMAKE_CXX_FLAGS="-luuid" \
	-DTTK_BUILD_STANDALONE_APPS=OFF \
	-DTTK_ENABLE_ZFP=ON \
	-DParaView_DIR:PATH=$BUILD_DIR/paraview-ttk \
	-DZFP_DIR:PATH=$BUILD_DIR/zfp \
	-DPYTHON_NUMPY_INCLUDE_DIR=$NUMPY_DIR \
	-DTTK_ENABLE_GRAPHVIZ=OFF \
	-DCMAKE_INSTALL_PREFIX=$INSTALL_DIR/ttk \
	-DCMAKE_BUILD_TYPE=Release \
	-DEIGEN_DIR=eigen/cmake \
	-DSPECTRA_INCLUDE_DIR=spectra/include \
	$CURRENT_DIR/ttk/
cmake --build . --target install --parallel "$NUM_BUILD_JOBS"
export PV_PLUGIN_PATH=$INSTALL_DIR/ttk/lib64/ttk/paraview-plugin

# finally switch to repository
cd $CURRENT_DIR
