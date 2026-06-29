
#!/usr/bin/env bash
set -e

cd  GeoModelSplitCal/build

./run_g4


# move output for Condor collection

mv output.root ../../EPWL_ECAL_Simulations/sim_runs


