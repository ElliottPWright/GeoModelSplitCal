#include "EventStore.hh"
#include "G4SystemOfUnits.hh"

void EventStore::clear() {
  edep.clear(); x_global.clear(); y_global.clear(); z_global.clear(); x_local.clear(); y_local.clear(); z_local.clear();
  type.clear(); section.clear(); layer.clear(); vol.clear(); hcal.clear();
}

void EventStore::addHit(const ParsedID& id, double e, const G4ThreeVector& p_global,const G4ThreeVector& p_local) {
  edep.push_back(e);         // in Geant4 energy units (MeV by default)
  x_global.push_back(p_global.x()); y_global.push_back(p_global.y()); z_global.push_back(p_global.z()); // in mm
  x_local.push_back(p_local.x()); y_local.push_back(p_local.y()); z_local.push_back(p_local.z()); // in mm
  type.push_back(id.type);
  section.push_back(id.section);
  layer.push_back(id.layer);
  vol.push_back(id.vol);
  hcal.push_back(id.hcal);
  hpl_sublayer.push_back(id.hpl_sublayer);
  hexant.push_back(id.hexant);
}
