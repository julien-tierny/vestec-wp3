#!/usr/bin/env bash
# launch with systemd-run --user -u=run_space_weather -d bash launch-runs.sh

CMAKE_PREFIX_PATH=/usr/local
TTK_BUILD_PATH=$HOME/Projects/ttk/build
PV_PLUGIN_PATH=/usr/bin/plugins

PYTHONPATH=/usr/lib/python3.8/site-packages:$CMAKE_PREFIX_PATH/lib/python3.8/site-packages
LD_LIBRARY_PATH=$PV_PLUGIN_PATH:$CMAKE_PREFIX_PATH/lib:$LD_LIBRARY_PATH

export LD_LIBRARY_PATH
export PYTHONPATH
export PV_PLUGIN_PATH
export CMAKE_PREFIX_PATH

sim_params=./ipicmini/inputfiles/GEM3D.inp
exe=./ipicmini/build/iPICmini

echo "B0z = 0.0" >> $sim_params
echo "ns = 4" >> $sim_params
$exe $sim_params

echo "B0z = 0.0195" >> $sim_params
echo "ns = 4" >> $sim_params
$exe $sim_params

echo "B0z = 0.0195" >> $sim_params
echo "ns = 2" >> $sim_params
$exe $sim_params

echo "B0z = 0.0" >> $sim_params
echo "ns = 2" >> $sim_params
$exe $sim_params

pvpython clusterDiags.py
