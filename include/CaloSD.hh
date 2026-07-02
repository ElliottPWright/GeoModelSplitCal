#pragma once
#include "G4VSensitiveDetector.hh"
#include "G4ThreeVector.hh"
#include <unordered_map>
#include <vector>
#include <string>

struct HitAgg {
  double edep = 0.0;
  G4ThreeVector sumEpos = {0,0,0};      // global
  G4ThreeVector sumEposLocal = {0,0,0}; // local to PV
};



enum VolumeType : int {
  kWideH=1, kWideV=2, kThinH=3, kThinV=4, kFibreH=5, kFibreV=6
};

enum SectionType : int { kEcal=0, kHcal=1 };

struct ParsedID {
  int type = -1;
  int section = -1;
  int layer = -1;
  int vol = -1;   // bar index or fibre index (your "volume number")
  int hcal = -1;
  int hpl_sublayer = -1;
  int hexant = 11;
};

class EventStore; // forward

class CaloSD : public G4VSensitiveDetector {
public:
  CaloSD(const G4String& name, EventStore* store);

  void Initialize(G4HCofThisEvent*) override;
  G4bool ProcessHits(G4Step* step, G4TouchableHistory*) override;
  void EndOfEvent(G4HCofThisEvent*) override;

private:
  EventStore* m_store = nullptr;
  std::unordered_map<std::string, HitAgg> m_map;

  static ParsedID parse(const std::string& name);
};
