#!/bin/bash

fileList=""
for((I=0; I < 45; I++)); do
  fileList="$fileList -i vortexStreet_diagrams/diagram_"$I".vtu"
done
ttkPersistenceDiagramsClusteringCmd $fileList -o VORTEXSTREET_clustering -d 30 -T 10 -K 5 -I 1 -A 1 -G 0.4 -P -1 -R 0

