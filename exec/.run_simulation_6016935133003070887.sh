
#!/usr/bin/env bash

source /cvmfs/sft.cern.ch/lcg/views/LCG_108/x86_64-el9-gcc13-opt/setup.sh

cd /eos/user/e/elwright/GeoModelSplitCal/build

cmake ..
make
./make_leadplate_db
./run_g4


