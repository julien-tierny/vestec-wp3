#!/bin/bash

fileList=""
for((I=0; I < 100; I++)); do
  fileList="$fileList -i ./gaussians_diagrams/diagram_"$I".vtu"
done

# for((J=0; J < 5; J++)); do
ttkPersistenceDiagramsClusteringCmd $fileList -o output_clustering_barycenter_gaussians -d 1 -T 10000000 -G 1.0 -K 1 -P -1 -A 1 -I 1 -t 1
# don

