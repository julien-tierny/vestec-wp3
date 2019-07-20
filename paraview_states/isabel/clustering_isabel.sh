#!/bin/bash

fileList=""
for((I=0; I < 12; I++)); do
  fileList="$fileList -i isabel_diagrams/diagram_"$I".vtu"
done

ttkPersistenceDiagramsClusteringCmd $fileList -o ISABEL_clustering_output -d 1 -T 10 -G 0.8 -K 3 -A 1 -I 1 
