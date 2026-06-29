
#!/usr/bin/env bash
set -e

echo "Running job $1"

source /cvmfs/sft.cern.ch/lcg/views/LCG_108/x86_64-el9-gcc13-opt/setup.sh

cd  GeoModelSplitCal/build

./run_g4


# move output for Condor collection

xrdcp -f  output.root root://eosuser.cern.ch//eos/home-e/elwright/EPWL_ECAL_Simulations/sim_runs/run0/output/output_$1.root

echo "Done"

