#!/bin/bash

mkdir ipicmini/build
cd ipicmini/build

cmake -DUSE_CATALYST=ON \
  -DCMAKE_BUILD_TYPE=Release \
  ../src

cmake --build . --parallel 4
