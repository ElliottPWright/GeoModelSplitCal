#pragma once
#include "G4ThreeVector.hh"
#include <vector>
#include "CaloSD.hh"

class EventStore {
public:
  void clear();

  void addHit(const ParsedID& id,
              double edep,
              const G4ThreeVector& pos);

  void addHit(const ParsedID&,
              double edep,
              const G4ThreeVector& posGlobal,
              const G4ThreeVector& posLocal);

  void addTrackPoint(int layer, const G4ThreeVector& pos);

  std::vector<double> edep, x_global, y_global, z_global;
  std::vector<double> x_local, y_local, z_local;

  std::vector<int> type, section, layer, vol, hcal,
                   hpl_sublayer, hexant;

  // Truth track points
   // reconstructed track points
  std::vector<int> track_layer;
  std::vector<double> track_x;
  std::vector<double> track_y;
  std::vector<double> track_z;
};
