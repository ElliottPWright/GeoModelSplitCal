#!/bin/bash
set -eo pipefail

PROCESS="${1:?Usage: $0 PROCESS_INDEX (0-4)}"

ENERGIES=(
5000 10000 20000 50000 100000
)

if [[ ! "$PROCESS" =~ ^[0-4]$ ]]; then
    echo "Invalid process index: $PROCESS (expected 0-4)" >&2
    exit 2
fi

ENERGY=${ENERGIES[$PROCESS]}
PROJECT_DIR=/afs/cern.ch/user/m/mahajanv/GeoModelSplitCal


source /cvmfs/sft.cern.ch/lcg/views/LCG_108/x86_64-el9-gcc13-opt/setup.sh

SCRATCH="${_CONDOR_SCRATCH_DIR:?HTCondor scratch directory is not set}"

mkdir -p "$SCRATCH"
WORKDIR="$SCRATCH/GeoModelSplitCal"
cp -a "$PROJECT_DIR" "$WORKDIR"

cd "$WORKDIR"

sed -i -E "s/^[[:space:]]*n_events[[:space:]]*=.*/n_events = 10000/" run.cfg
sed -i -E "s/^[[:space:]]*energy_MeV[[:space:]]*=.*/energy_MeV = ${ENERGY}/" run.cfg
sed -i -E "s/^[[:space:]]*particle[[:space:]]*=.*/particle = e-/" run.cfg
sed -i -E "s/^[[:space:]]*visualize[[:space:]]*=.*/visualize = 0/" run.cfg

# Copy macro and config files into build directory so Geant4 can access them
cp run.mac build/

cd "$WORKDIR/build"

chmod +x run_g4

./run_g4

ROOT_FILE="calosim_out_${ENERGY}MeV_e-.root"
if [[ ! -s "$ROOT_FILE" ]]; then
    echo "ERROR: Expected ROOT output was not produced: $ROOT_FILE" >&2
    exit 1
fi

OUT_DIR="/eos/user/m/mahajanv/GeoModelSplitCal/sim_results"
mkdir -p "$OUT_DIR"
OUTPUT_FILE="$OUT_DIR/sim_e-_10k_${ENERGY}MeV.root"
if [[ -e "$OUTPUT_FILE" ]]; then
    echo "Overwriting existing output: $OUTPUT_FILE"
    rm -f "$OUTPUT_FILE"
fi
mv "$ROOT_FILE" "$OUTPUT_FILE"
echo "Saved $OUTPUT_FILE"
