#!/bin/bash

fileList=""
for((I=0; I < 48; I++)); do
  fileList="$fileList -i seaSurfaceHeight_diagrams/diagram_"$I".vtu"
done
ttkPersistenceDiagramsClusteringCmd $fileList -o SSH_clustering_output -d 4 -T 100 -K 4 -A 1 -I 1 -G 0.8
