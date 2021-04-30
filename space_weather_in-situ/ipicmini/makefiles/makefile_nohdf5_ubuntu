#
# Makefile for iPic3D mini-app without HDF5
# V. Olshevsky 2018
#
CPP = mpicxx

include = -I./src/include/
INC_MPI = -I/usr/lib/openmpi/include/

OPTFLAGS = -lstdc++ -O3 -DNO_HDF5
#OPTFLAGS = -O3 -DNO_HDF5 -fopt-info-vec -pg -I${include}

objects = iPIC3Dlib.o Parameters.o iPICmini.o Collective.o VCtopology3D.o Com3DNonblk.o Grid3DCU.o \
          EMfields3D.o Particles3Dcomm.o Particles3D.o ConfigFile.o TimeTasks.o IDgenerator.o MPIdata.o \
          Timing.o ParallelIO.o debug.o asserts.o errors.o BcFields3D.o Basic.o Moments.o EllipticF.o \
          GMRES.o

ipic-mini: ${objects}
	${CPP} ${OPTFLAGS} -o iPICmini ${INC_MPI} ${INC_HDF5} ${objects} ${LIB_MPI} ${LIB_HDF5}

iPICmini.o: ./src/iPICmini.cpp
	${CPP} ${OPTFLAGS} ${include} ${INC_MPI} ${INC_HDF5} -c ./src/iPICmini.cpp 

Collective.o: ./src/main/Collective.cpp
	${CPP} ${OPTFLAGS} ${include} ${INC_MPI} ${INC_HDF5} -c ./src/main/Collective.cpp

iPIC3Dlib.o: ./src/main/iPIC3Dlib.cpp
	${CPP} ${OPTFLAGS} ${include} ${INC_MPI} ${INC_HDF5} -c ./src/main/iPIC3Dlib.cpp

Parameters.o: ./src/main/Parameters.cpp
	${CPP} ${OPTFLAGS} ${include} -c ./src/main/Parameters.cpp

VCtopology3D.o: ./src/communication/VCtopology3D.cpp
	${CPP} ${OPTFLAGS} ${include} -c ./src/communication/VCtopology3D.cpp

Com3DNonblk.o: ./src/communication/Com3DNonblk.cpp
	${CPP} ${OPTFLAGS} ${include} -c ./src/communication/Com3DNonblk.cpp

Grid3DCU.o: ./src/grids/Grid3DCU.cpp Com3DNonblk.o
	${CPP} ${OPTFLAGS} ${include} -c ./src/grids/Grid3DCU.cpp

EMfields3D.o: ./src/fields/EMfields3D.cpp
	${CPP} ${OPTFLAGS} ${include} ${INC_MPI} ${INC_HDF5} -c ./src/fields/EMfields3D.cpp

Moments.o: ./src/fields/Moments.cpp
	${CPP} ${OPTFLAGS} ${include} -c ./src/fields/Moments.cpp

MPIdata.o: ./src/utility/MPIdata.cpp
	${CPP} ${OPTFLAGS} ${include} -c ./src/utility/MPIdata.cpp

TimeTasks.o: ./src/utility/TimeTasks.cpp
	${CPP} ${OPTFLAGS} ${include} -c ./src/utility/TimeTasks.cpp

IDgenerator.o: ./src/utility/IDgenerator.cpp
	${CPP} ${OPTFLAGS} ${include} -c ./src/utility/IDgenerator.cpp

Basic.o: ./src/utility/Basic.cpp
	${CPP} ${OPTFLAGS} ${include} -c ./src/utility/Basic.cpp

debug.o: ./src/utility/debug.cpp
	${CPP} ${OPTFLAGS} ${include} -c ./src/utility/debug.cpp

errors.o: ./src/utility/errors.cpp
	${CPP} ${OPTFLAGS} ${include} -c ./src/utility/errors.cpp

asserts.o: ./src/utility/asserts.cpp
	${CPP} ${OPTFLAGS} ${include} -c ./src/utility/asserts.cpp

ConfigFile.o: ./src/ConfigFile/ConfigFile.cpp
	${CPP} ${OPTFLAGS} ${include} -c ./src/ConfigFile/ConfigFile.cpp

Timing.o: ./src/performances/Timing.cpp
	${CPP} ${OPTFLAGS} ${include} -c ./src/performances/Timing.cpp

Particles3Dcomm.o: ./src/particles/Particles3Dcomm.cpp
	${CPP} ${OPTFLAGS} ${include} ${INC_MPI} ${INC_HDF5} -c ./src/particles/Particles3Dcomm.cpp

Particles3D.o: ./src/particles/Particles3D.cpp 
	${CPP} ${OPTFLAGS} ${include} -c ./src/particles/Particles3D.cpp

ParallelIO.o: ./src/inputoutput/ParallelIO.cpp
	${CPP} ${OPTFLAGS} ${include} -c ./src/inputoutput/ParallelIO.cpp

BcFields3D.o: ./src/bc/BcFields3D.cpp
	${CPP} ${OPTFLAGS} ${include} -c ./src/bc/BcFields3D.cpp

EllipticF.o: ./src/mathlib/EllipticF.cpp
	${CPP} ${OPTFLAGS} ${include} -c ./src/mathlib/EllipticF.cpp

GMRES.o: ./src/solvers/GMRES.cpp
	${CPP} ${OPTFLAGS} ${include} -c ./src/solvers/GMRES.cpp

.PHONY : clean
clean:
	rm -rf *.o iPICmini
