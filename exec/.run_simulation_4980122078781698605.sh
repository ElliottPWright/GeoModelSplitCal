
#!/usr/bin/env bash

source /cvmfs/sft.cern.ch/lcg/views/LCG_108/x86_64-el9-gcc13-opt/setup.sh

# Replace line with /random/setSeeds with random seeds, or add if missing
RANDOM1=$RANDOM
RANDOM2=$RANDOM
sed "s=/random/setSeeds.*=/random/setSeeds $RANDOM1 $RANDOM2=" $1 > my_simulation.mac

if [ ! $(grep -q "/random/setSeeds" $1) ]; then
    sed -i "1s=^=/random/setSeeds $RANDOM1 $RANDOM2\n=" my_simulation.mac
fi

>&2 echo "RANDOM SEEDS = ${{RANDOM1}} ${{RANDOM2}}"
cat my_simulation.mac
./run_g4 my_simulation.mac


