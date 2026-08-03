# Introduction

Welcome to the GeoModelSplitCal repository, a standalone simulation for the SHiP ECAL + HCAL. The first thing you should do is consult building_dependencies.md so you can run a simulation effectively. For an understanding of the structure of the simulation, you can read GeoModelSplitCal_Documentation.pdf (still a work-in-progress).

Analysis for the outputs of these simulation can be conducted in [ECAL_Simulation_Analysis](https://github.com/ElliottPWright/ECAL_Simulation_Analysis/tree/main)


# Current Jobs

- Implementation of appropriate materials into the module gaps
- Update the documentation to make it a more useful reference point


# Instructions for geometry display (still Matei's)

```
mkdir build

cd build

cmake ..

make

./make_leadplate_db

gmex geometry.db
```

changes in calo.cfg should be reflected in the geometry after running


# Instructions for simulation run 

```
mkdir build

cd build

cmake ..

make

./run_g4
```

It should be noted that the coordinate system for the detector placement originates at 0,0,0 at the target region

You can change parameters in .cfg files

Visualisation can be added through visualize = 1 (enable visualisation)

| Mode | Description                      | Speed   |
| ---- | -------------------------------- | ------- |
| 0    | world + module envelopes only    | fastest |
| 1    | world + module + layer envelopes | medium  |
| 2    | full detail, all bars/fibres     | slowest |

