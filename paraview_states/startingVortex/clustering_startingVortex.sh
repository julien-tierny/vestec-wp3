#!/bin/bash

fileList=""
for((I=0; I < 12; I++)); do
  fileList="$fileList -i startingVortex_diagrams/diagram_"$I".vtu"
done
# /home/julius/ttk/build_debug/standalone/PersistenceDiagramsClustering/cmd/ttkPersistenceDiagramsClusteringCmd $fileList -o output_clustering_startingVortex -d 1 -T 10 -K 2 -A 1 -I 1 -t 1 -R 0
ttkPersistenceDiagramsClusteringCmd $fileList -o STARTINGVORTEX_clustering_output -d 30 -T 10 -K 2 -A 1 -I 1 -t 1 -R 0 -G 0.5

