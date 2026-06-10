#include "EventAction.hh"
#include "G4AnalysisManager.hh"
#include "G4Event.hh"
#include "G4ios.hh"
#include "G4TrajectoryContainer.hh"
#include "G4Trajectory.hh"
#include "G4TrajectoryPoint.hh"
#include "G4PrimaryVertex.hh"
#include "G4PrimaryParticle.hh"
#include "CLHEP/Units/SystemOfUnits.h"


EventAction::EventAction(EventStore* store) : m_store(store) {
  auto* man = G4AnalysisManager::Instance();

  man->CreateNtuple("calo_events", "calo hits");
  man->CreateNtupleDColumn("edep", m_store->edep);
  man->CreateNtupleDColumn("x_global",    m_store->x_global);
  man->CreateNtupleDColumn("y_global",    m_store->y_global);
  man->CreateNtupleDColumn("z_global",    m_store->z_global);
  man->CreateNtupleDColumn("x_local",     m_store->x_local);
  man->CreateNtupleDColumn("y_local",     m_store->y_local);
  man->CreateNtupleDColumn("z_local",     m_store->z_local);

  man->CreateNtupleIColumn("type",    m_store->type);
  man->CreateNtupleIColumn("section", m_store->section);
  man->CreateNtupleIColumn("layer",   m_store->layer);
  man->CreateNtupleIColumn("vol",     m_store->vol);
  man->CreateNtupleIColumn("hcal",     m_store->hcal);
  man->CreateNtupleIColumn("hpl_subsection",     m_store->hpl_sublayer);
  man->CreateNtupleIColumn("hexant",     m_store->hexant);

  man->FinishNtuple();

  // For track events
  man->CreateNtuple("track_events", "track points");

  man->CreateNtupleIColumn("track_layer", m_store->track_layer);

  man->CreateNtupleDColumn("track_x", m_store->track_x);
  man->CreateNtupleDColumn("track_y", m_store->track_y);
  man->CreateNtupleDColumn("track_z", m_store->track_z);

  man->FinishNtuple();
  
}

void EventAction::BeginOfEventAction(const G4Event* event) {
  m_store->clear();
  G4cout << "\n=== Event " << event->GetEventID() << " ===" << G4endl;
}


void EventAction::EndOfEventAction(const G4Event* event) {
  // Print primary vertex
  //auto* vtx = event->GetPrimaryVertex();
  //while (vtx) {
  //  auto* particle = vtx->GetPrimary();
  //  while (particle) {
  //    G4cout << "[Primary] PDG=" << particle->GetPDGcode()
  //           << " E=" << particle->GetTotalEnergy()/CLHEP::MeV << " MeV"
  //           << " pos=(" << vtx->GetX0()/CLHEP::mm
  //           << ", "     << vtx->GetY0()/CLHEP::mm
  //           << ", "     << vtx->GetZ0()/CLHEP::mm << ") mm"
  //           << G4endl;
  //    particle = particle->GetNext();
  //  }
  //  vtx = vtx->GetNext();
  //}

  // Print pi0 decay photons from trajectory container
  //auto* trajContainer = event->GetTrajectoryContainer();
  //if (trajContainer) {
  //  int nPhotons = 0;
  //  for (size_t i = 0; i < trajContainer->size(); ++i) {
  //    auto* traj = static_cast<G4Trajectory*>((*trajContainer)[i]);
  //    if (traj->GetPDGEncoding() == 22 && traj->GetParentID() > 0) {
  //      auto* startPoint = static_cast<G4TrajectoryPoint*>(traj->GetPoint(0));
  //      G4ThreeVector pos = startPoint->GetPosition();
  //      G4cout << "[Photon] E=" << traj->GetInitialKineticEnergy()/CLHEP::MeV << " MeV"
  //             << " origin=(" << pos.x()/CLHEP::mm
  //             << ", "        << pos.y()/CLHEP::mm
  //             << ", "        << pos.z()/CLHEP::mm << ") mm"
  //             << G4endl;
  //      ++nPhotons;
  //    }
  //  }
  //  G4cout << "[Event] N photons from decay: " << nPhotons << G4endl;
  //} else {
  //  G4cout << "[Event] No trajectory container — add /tracking/storeTrajectory 1 to your vis.mac or macro" << G4endl;
  //}

  //G4cout << "[Event] Total hits: " << m_store->edep.size() << G4endl;
  G4AnalysisManager::Instance()->AddNtupleRow();
}
