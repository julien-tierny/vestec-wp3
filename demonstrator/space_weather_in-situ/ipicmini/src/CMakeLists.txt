cmake_minimum_required(VERSION 3.8)

project(iPICmini C CXX)

set(CMAKE_CXX_STANDARD 11)

set(IPIC_INCLUDES ${CMAKE_SOURCE_DIR}/include)
set(IPIC_LIBRARIES "")
set(IPIC_DEFINITIONS "")
add_definitions(-DOMPI_SKIP_MPICXX)

# Find MPI
find_package(MPI REQUIRED COMPONENTS CXX)

# Find HDF5
option(USE_HDF5 "Use HDF5 library" OFF)
if(USE_HDF5)
  find_package(HDF5 REQUIRED)
  list(APPEND IPIC_LIBRARIES ${HDF5_LIBRARIES})
else()
  list(APPEND IPIC_DEFINITIONS -DNO_HDF5)
endif()

# Find ParaView
option(USE_CATALYST "Use Catalyst adaptor" ON)
if(USE_CATALYST)
  find_package(ParaView 5.7 REQUIRED)

  if (NOT TARGET ParaView::PythonCatalyst)
    message(STATUS
        "${CMAKE_PROJECT_NAME} requires ParaView to be built with Catalyst and "
        "Python support enabled. Please rebuild ParaView (or point to a "
        "different build of ParaView) with PARAVIEW_ENABLE_CATALYST and "
        "PARAVIEW_ENABLE_PYTHON set to TRUE")
  else()
    add_library(iPICAdaptor SHARED ${CMAKE_SOURCE_DIR}/catalyst/Adaptor.cxx)
    target_link_libraries(iPICAdaptor PUBLIC
      ParaView::PythonCatalyst
      VTK::CommonDataModel
      MPI::MPI_CXX
      )

    list(APPEND IPIC_LIBRARIES iPICAdaptor)
    list(APPEND IPIC_DEFINITIONS -DUSE_CATALYST)
    list(APPEND IPIC_INCLUDES ${CMAKE_SOURCE_DIR}/catalyst)
  endif()
endif()

# Particle solver
add_executable(
  iPICmini
  ${CMAKE_SOURCE_DIR}/iPICmini.cpp
)

add_library(
  iPIC3Dlib
  SHARED
  ${CMAKE_SOURCE_DIR}/iPICmini.cpp
  ${CMAKE_SOURCE_DIR}/main/Collective.cpp
  ${CMAKE_SOURCE_DIR}/main/iPIC3Dlib.cpp
  ${CMAKE_SOURCE_DIR}/main/Parameters.cpp
  ${CMAKE_SOURCE_DIR}/communication/VCtopology3D.cpp
  ${CMAKE_SOURCE_DIR}/communication/Com3DNonblk.cpp
  ${CMAKE_SOURCE_DIR}/grids/Grid3DCU.cpp
  ${CMAKE_SOURCE_DIR}/fields/EMfields3D.cpp
  ${CMAKE_SOURCE_DIR}/fields/Moments.cpp
  ${CMAKE_SOURCE_DIR}/utility/MPIdata.cpp
  ${CMAKE_SOURCE_DIR}/utility/TimeTasks.cpp
  ${CMAKE_SOURCE_DIR}/utility/IDgenerator.cpp
  ${CMAKE_SOURCE_DIR}/utility/Basic.cpp
  ${CMAKE_SOURCE_DIR}/utility/debug.cpp
  ${CMAKE_SOURCE_DIR}/utility/errors.cpp
  ${CMAKE_SOURCE_DIR}/utility/asserts.cpp
  ${CMAKE_SOURCE_DIR}/ConfigFile/ConfigFile.cpp
  ${CMAKE_SOURCE_DIR}/performances/Timing.cpp
  ${CMAKE_SOURCE_DIR}/particles/Particles3Dcomm.cpp
  ${CMAKE_SOURCE_DIR}/particles/Particles3D.cpp
  ${CMAKE_SOURCE_DIR}/inputoutput/ParallelIO.cpp
  ${CMAKE_SOURCE_DIR}/bc/BcFields3D.cpp
  ${CMAKE_SOURCE_DIR}/mathlib/EllipticF.cpp
  ${CMAKE_SOURCE_DIR}/solvers/GMRES.cpp
)

target_link_libraries(iPIC3Dlib PUBLIC ${IPIC_LIBRARIES})
target_include_directories(iPIC3Dlib PUBLIC ${IPIC_INCLUDES})
target_compile_definitions(iPIC3Dlib PUBLIC ${IPIC_DEFINITIONS})

target_link_libraries(iPICmini PUBLIC iPIC3Dlib)

# Find OpenMP
find_package(OpenMP)
if(OpenMP_CXX_FOUND)
  target_link_libraries(iPIC3Dlib PUBLIC OpenMP::OpenMP_CXX)
endif()

## debug releases have a _d appended to the executable
set_target_properties(iPICmini PROPERTIES DEBUG_POSTFIX "_d")
