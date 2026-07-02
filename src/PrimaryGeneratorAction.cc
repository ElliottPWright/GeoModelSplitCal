#include "PrimaryGeneratorAction.hh"

#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4Event.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"

PrimaryGeneratorAction::PrimaryGeneratorAction(const RunConfig& cfg)
: m_cfg(cfg), m_gun(std::make_unique<G4ParticleGun>(1))
{
  auto* table = G4ParticleTable::GetParticleTable();
  auto* def = table->FindParticle(m_cfg.particle);
  if (!def) {
    G4Exception("PrimaryGeneratorAction", "BadParticle", FatalException,
                ("Unknown particle: " + m_cfg.particle).c_str());
  }

  m_gun->SetParticleDefinition(def);
  m_gun->SetParticleEnergy(m_cfg.energy_MeV * MeV);

}

PrimaryGeneratorAction::~PrimaryGeneratorAction() = default;

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* event)
{
 // Roll fresh position and direction every event
 double x = (G4UniformRand() * 4000) - 2000;
 double y = (G4UniformRand() * 6000) - 3000;
 double z = (m_cfg.position_mm[2] - 0.5 * 120827.0) * mm;

 double dx = (G4UniformRand() * 4000) - x - 2000;
 double dy = (G4UniformRand() * 6000) - y - 3000;

 if (m_cfg.sigma_xy_mm > 0) {
    x += G4RandGauss::shoot(0.0, m_cfg.sigma_xy_mm * mm);
    y += G4RandGauss::shoot(0.0, m_cfg.sigma_xy_mm * mm);
  }

  m_gun->SetParticleMomentumDirection(
    G4ThreeVector(dx, dy, 65530).unit()
  );

  m_gun->SetParticlePosition(G4ThreeVector(x,y,z));
  m_gun->GeneratePrimaryVertex(event);
}
