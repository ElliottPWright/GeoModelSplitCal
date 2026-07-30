#Instructions for geometry display

mkdir build
cd build
cmake ..
make
./make_leadplate_db
gmex geometry.db

changes in calo.cfg should be reflected in the geometry after running


#Instruction for simulation run

mkdir build
cd build
cmake ..
make
./run_g4

It should be noted that the coordinate system for the detector placement originates at 0,0,0 at the target region

You can change parameters in .cfg files

Visualisation can be added through visualize = 1 (enable visualisation)

0 (default) : world + module envelopes only          (fastest)
1           : world + module + layer envelopes       (medium)  
2           : full detail, all bars/fibres           (slowest)
