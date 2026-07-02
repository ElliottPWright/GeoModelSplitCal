#include "CaloSD.hh"
#include "G4Step.hh"
#include "G4VTouchable.hh"
#include "G4TouchableHistory.hh"
#include "G4SystemOfUnits.hh"
#include "G4Track.hh"
#include <regex>

#include "EventStore.hh"

CaloSD::CaloSD(const G4String& name, EventStore* store)
: G4VSensitiveDetector(name), m_store(store) {}

void CaloSD::Initialize(G4HCofThisEvent*) {
  m_map.clear();
}

G4bool CaloSD::ProcessHits(G4Step* step, G4TouchableHistory*) {

  const auto pre  = step->GetPreStepPoint()->GetPosition();
  const auto post = step->GetPostStepPoint()->GetPosition();
  const auto mid  = 0.5*(pre + post);


  const auto& touch = step->GetPreStepPoint()->GetTouchableHandle();
  const auto local = touch->GetHistory()->GetTopTransform().TransformPoint(mid);
  const auto* pv = touch->GetVolume();
  const auto edep = step->GetTotalEnergyDeposit();
  if (!pv) return false;
  
  const std::string vname = pv->GetName();

  if (edep <= 0) return false;
  
  
  
  auto& agg = m_map[vname];
  agg.edep += edep;
  agg.sumEpos += edep * mid;
  agg.sumEposLocal += edep * local;


  static int nh = 0;
//  if (edep > 0) {
//    G4cout << "Hit "<< nh <<" in PV=" << pv->GetName()
//           << " edep(MeV)=" << edep/CLHEP::MeV << G4endl;
//    ++nh;
//  }

 // const auto* tr = step->GetTrack();
 // const auto  pre = step->GetPreStepPoint();
 // const auto  post = step->GetPostStepPoint();
 // const auto  zpre = pre->GetPosition().z()/CLHEP::mm;
 // const auto  zpost = post->GetPosition().z()/CLHEP::mm;
 // const auto  dz = zpost - zpre;

 // G4cout << "PV=" << pv->GetName()
 //        << " edep(MeV)=" << edep/CLHEP::MeV
 //        << " zpre(mm)=" << zpre
 //        << " zpost(mm)=" << zpost
 //        << " dz(mm)=" << dz
 //        << " particle=" << tr->GetDefinition()->GetParticleName()
 //        << " trackID=" << tr->GetTrackID()
 //        << " parentID=" << tr->GetParentID()
 //        << G4endl;



  return true;
}

void CaloSD::EndOfEvent(G4HCofThisEvent*) {

  for (const auto& [name, agg] : m_map) {
    if (agg.edep <= 0) continue;
    const auto id = parse(name);
    const auto posGlobal = agg.sumEpos / agg.edep;
    const auto posLocal  = agg.sumEposLocal / agg.edep;
    m_store->addHit(id, agg.edep, posGlobal, posLocal);
  }

}

ParsedID CaloSD::parse(const std::string& name) {
  ParsedID out;

  out.section = (name.rfind("HCAL_",0)==0) ? kHcal : kEcal;

  if (name.find("WidePVT_H_") != std::string::npos) out.type = kWideH;
  else if (name.find("WidePVT_V_") != std::string::npos) out.type = kWideV;
  else if (name.find("ThinPS_H_") != std::string::npos) out.type = kThinH;
  else if (name.find("ThinPS_V_") != std::string::npos) out.type = kThinV;
  else if (name.find("HPL_H") != std::string::npos) out.type = kFibreH; 
  else if (name.find("HPL_V") != std::string::npos) out.type = kFibreV; 
  else {out.type = -1; std::cout << "NAME "<< name << std::endl;}

  if (name.find("ECAL_") != std::string::npos) out.hcal = 0;
  else if(name.find("HCAL_") != std::string::npos) out.hcal = 1;
  else out.hcal = -1;
  // extract L<number>
  {
    std::regex re("_SL([0-9]+)");
    std::smatch m;
    if (std::regex_search(name, m, re)) out.layer = std::stoi(m[1]);
  }

  // "volume number": for bars use B<number>, for fibres use F<number>
  {
    std::regex reB("_B([0-9]+)");
    std::smatch m;
    if (std::regex_search(name, m, reB)) out.vol = std::stoi(m[1]);
  }
  {
    std::regex reF("_F([0-9]+)");
    std::smatch m;
    if (std::regex_search(name, m, reF)) out.vol = std::stoi(m[1]);
  }
  {
    std::regex reF("_S([0-9]+)");
    std::smatch m;
    if (std::regex_search(name, m, reF)) out.hpl_sublayer = std::stoi(m[1]);
  }

  {
    std::regex re("_MX([0-9]+)Y([0-9]+)");
    std::smatch m;
    if (std::regex_search(name, m, re)) {
      const int mx = std::stoi(m[1]);
      const int my = std::stoi(m[2]);
      out.hexant = 10*mx + my;
    } else {
      out.hexant = 11; // fallback
    }
  }


  return out;
}
